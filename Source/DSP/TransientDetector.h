#pragma once

#include <cmath>

class TransientDetector
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        release = std::exp(-1.0f / (0.01f * static_cast<float>(sr)));
    }

    void setSensitivity(float s)
    {
        threshold = 0.001f + (1.0f - s) * 0.5f;
    }

    bool process(float sample)
    {
        const float diff = std::abs(sample - prev);
        prev = sample;
        envelope = std::max(diff, envelope * release);
        return envelope > threshold;
    }

private:
    double sampleRate = 44100.0;
    float prev = 0.0f;
    float envelope = 0.0f;
    float threshold = 0.1f;
    float release = 0.99f;
};
