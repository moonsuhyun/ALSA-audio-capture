#ifndef __SRC_AUDIOCAPTURE_HPP__
#define __SRC_AUDIOCAPTURE_HPP__

#include <alsa/asoundlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <csignal>
#include <atomic>

#define ThrowUnderZero(_Cond_, _Code_)\
    do {\
        _Cond_ = _Code_;\
        if (_Cond_ < 0) throw #_Code_;\
    } while (0U);\
\

typedef struct Config {
    std::string device;
    uint32_t rate;
    uint32_t channels;
    snd_pcm_format_t format;
    uint32_t sec;
} Config_t;

class AudioCapture {
private:
    Config_t cfg;
    snd_pcm_t* pcm;
    snd_pcm_hw_params_t* hw;
    std::vector<int16_t> buffer;
    std::vector<std::vector<int16_t>> input;
    int32_t offset;
    int32_t pcmInit(void);
    int32_t pcmPrepare(void);
    int32_t pcmCleanup(void);
    static std::atomic<bool> run;
public:
    AudioCapture(void) : AudioCapture("default", 48000, 1, SND_PCM_FORMAT_S16_LE, 5) {}
    AudioCapture(std::string device, uint32_t rate, uint32_t channel, snd_pcm_format_t format, uint32_t sec);
    void Capture(void);
};

#endif /* SRC_AUDIOCAPTURE_HPP */
