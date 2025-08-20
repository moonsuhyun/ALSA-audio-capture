#ifndef __SRC_INPUTAUDIO_HPP__
#define __SRC_INPUTAUDIO_HPP__

#include <asoundlib.h>
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

class InputAudio {
private:
    Config_t cfg;
    snd_pcm_t* pcm;
    snd_pcm_hw_params_t* hw;
    std::vector<int16_t> buffer;
    int32_t pcmInit(void);
    int32_t pcmPrepare(void);
    int32_t pcmCleanup(void);
    static std::atomic<bool> run;
public:
    InputAudio(void) : InputAudio("hw:0,0", 48000, 1, SND_PCM_FORMAT_S16_LE, 30) {}
    InputAudio(std::string device, uint32_t rate, uint32_t channel, snd_pcm_format_t format, uint32_t sec);
    void Capture(void);
};

#endif /* SRC_INPUTAUDIO_HPP */