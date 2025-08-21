#include <iostream>
#include "AudioCapture.hpp"

int main() {
    AudioCapture ac = AudioCapture();

    auto input = ac.Capture();

    return 0;
}