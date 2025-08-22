#include <iostream>
#include <fstream>
#include "AudioCapture.hpp"

int main() {
    AudioCapture ac = AudioCapture();

    auto input = ac.Capture();

    // test code
    uint32_t idx = 0;
    for (auto audio : input)
    {
        std::cout << "\rFile #" << idx + 1 << " of " << input.size() << " processing..." << std::flush;
        std::ofstream raw_file("output" + std::to_string(idx++) + ".raw", std::ios::binary);
        if (raw_file.is_open()) raw_file.write(reinterpret_cast<const char*>(audio.data()), audio.size() * sizeof(int16_t));
        // aplay -r 48000 -f S16_LE -c 1 output.raw
        raw_file.close();
    }
    return 0;
}