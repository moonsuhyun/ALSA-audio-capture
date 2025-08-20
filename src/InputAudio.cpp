#include "InputAudio.hpp"

InputAudio::InputAudio(std::string device, uint32_t rate, uint32_t channel, snd_pcm_format_t format, uint32_t sec)
    : cfg{device, rate, channel, format, sec} {
    pcm = nullptr;
    hw = nullptr;
    buffer = std::vector<int16_t> (cfg.rate * cfg.sec, 0);
}

int32_t InputAudio::pcmInit(void) {
    int32_t rc;

    try {
        // open audio device
        ThrowUnderZero(rc, snd_pcm_open(&pcm, cfg.device.c_str(), SND_PCM_STREAM_CAPTURE, 0))

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

int32_t InputAudio::pcmPrepare(void) {
    int32_t rc;
    try {
        ThrowUnderZero(rc, snd_pcm_prepare(pcm));
    } catch (std::string code) {
        std::cerr << code << ": " << snd_strerror(rc) << std::endl;
        return -1;
    }
    return 0;
}

int32_t InputAudio::pcmCleanup(void) {
    snd_pcm_close(pcm);
    return 0;
}

void InputAudio::Capture(void) {
    int32_t rc;
    InputAudio::run = true;

    std::signal(SIGINT, [](int) -> void { InputAudio::run = false; });

    try {
        ThrowUnderZero(rc, pcmInit());
        ThrowUnderZero(rc, pcmPrepare());

        while (InputAudio::run) {
            // 오디오 캡쳐 시작
        }

    } catch (std::string code) {
        std::cerr << "Capture failed: cannot execute " << code << std::endl;
        return;
    }
}