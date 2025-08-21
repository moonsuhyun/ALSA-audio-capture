#include "AudioCapture.hpp"

std::atomic<bool> AudioCapture::run(true);

AudioCapture::AudioCapture(std::string device, uint32_t rate, uint32_t channel, snd_pcm_format_t format, uint32_t sec)
    : cfg{device, rate, channel, format, sec}, buffer(rate * sec, 0) {
    pcm = nullptr;
    hw = nullptr;
    offset = 0;
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

void AudioCapture::Capture(void) {
    int32_t rc;
    AudioCapture::run = true;
    int32_t buffer_size = cfg.rate * cfg.sec * 2;
    bool buffer_area_flag = false; // false: 0~29s, true: 30~59s

    std::signal(SIGINT, [](int) -> void { AudioCapture::run = false; });

    try {
        ThrowUnderZero(rc, pcmInit());
        ThrowUnderZero(rc, pcmPrepare());

        while (AudioCapture::run) {
            if (buffer_area_flag ^ (offset % buffer_size < cfg.rate * cfg.sec)) {
                auto ret = snd_pcm_readi(pcm, &buffer[offset % buffer_size], cfg.rate);
                if (ret < 0) {
                    if (ret == -EPIPE) pcmPrepare();
                    continue;
                }
                offset += ret;
            } else {
                auto half = buffer.begin() + cfg.rate * cfg.sec;

                auto first = !buffer_area_flag ? buffer.begin() : half;
                auto last = !buffer_area_flag ? half : buffer.end();

                input.emplace_back(first, last);
                
                std::fill(first, last, 0);

                buffer_area_flag = !buffer_area_flag;
            }
        }

        std::cout << "SIGINT: Audio capture aborted." << std::endl;
        auto half = buffer.begin() + cfg.rate * cfg.sec;

        auto first = !buffer_area_flag ? buffer.begin() : half;
        auto last = !buffer_area_flag ? half : buffer.end();

        input.emplace_back(first, last);

        pcmCleanup();

    } catch (std::string code) {
        std::cerr << "Capture failed: cannot execute " << code << std::endl;
        return;
    }
}