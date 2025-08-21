#include "AudioCapture.hpp"

std::atomic<bool> AudioCapture::run(true);

AudioCapture::AudioCapture(std::string device, uint32_t rate, uint32_t channel, snd_pcm_format_t format, uint32_t sec)
    : cfg{device, rate, channel, format, sec} {
    pcm = nullptr;
    hw = nullptr;
}

int32_t AudioCapture::pcmInit(void) {
    int32_t rc;

    try {
        // open audio device
        ThrowUnderZero(rc, snd_pcm_open(&pcm, cfg.device.c_str(), SND_PCM_STREAM_CAPTURE, 0));
        // allocate hw param struct
        ThrowUnderZero(rc, rc = snd_pcm_hw_params_malloc(&hw));
        // initialize hw param struct
        ThrowUnderZero(rc, snd_pcm_hw_params_any(pcm, hw));
        // set access type
        ThrowUnderZero(rc, snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED));
        // set sample format
        ThrowUnderZero(rc, snd_pcm_hw_params_set_format(pcm, hw, cfg.format));
        // set resample rate
        ThrowUnderZero(rc, snd_pcm_hw_params_set_rate_resample(pcm, hw, 1));
        // set sample rate
        ThrowUnderZero(rc, snd_pcm_hw_params_set_rate_near(pcm, hw, &cfg.rate, 0));
        // set channels
        ThrowUnderZero(rc, snd_pcm_hw_params_set_channels(pcm, hw, cfg.channels));
        // set hw params
        ThrowUnderZero(rc, snd_pcm_hw_params(pcm, hw));

    } catch (std::string code) {
        std::cerr << code << ": " << snd_strerror(rc) << std::endl;
        return -1;
    }

    snd_pcm_hw_params_free(hw);

    return 0;
}

int32_t AudioCapture::pcmPrepare(void) {
    int32_t rc;
    try {
        ThrowUnderZero(rc, snd_pcm_prepare(pcm));
    } catch (std::string code) {
        std::cerr << code << ": " << snd_strerror(rc) << std::endl;
        return -1;
    }
    return 0;
}

int32_t AudioCapture::pcmCleanup(void) {
    snd_pcm_close(pcm);
    return 0;
}

std::vector<std::vector<int16_t>>& AudioCapture::Capture(void) {
    std::cout << "Capture start..." << std::endl;
    int32_t rc;
    int32_t offset = 0;
    AudioCapture::run = true;
    int32_t buffer_size = cfg.rate * cfg.sec;
    std::vector<int16_t> buffer(buffer_size);
    std::vector<int16_t> temp_buffer(cfg.rate, 0);

    std::signal(SIGINT, [](int) -> void { AudioCapture::run = false; });

    try {
        ThrowUnderZero(rc, pcmInit());
        ThrowUnderZero(rc, pcmPrepare());

        while (AudioCapture::run) {
            std::cout << offset << std::endl;
            auto ret = snd_pcm_readi(pcm, temp_buffer.data(), cfg.rate);

            if (ret < 0) {
                if (ret == -EPIPE) pcmPrepare();
                std::cout << "Broken pipe" << std::endl;
                continue;
            }

            std::copy(temp_buffer.begin(), temp_buffer.begin() + ret, buffer.begin() + offset);
            offset += cfg.rate;

            if (offset == buffer_size) {
                input.emplace_back(buffer.begin(), buffer.end());
                offset = 0;
            }
        }

        std::cout << "SIGINT: Capture aborted." << std::endl;

        if (offset > 0) input.emplace_back(buffer.begin(), buffer.begin() + offset);

        pcmCleanup();

        std::cout << "Capture finished." << std::endl;

    } catch (std::string code) {
        std::cerr << "Capture failed: cannot execute " << code << std::endl;
    }
    
    return input;
}