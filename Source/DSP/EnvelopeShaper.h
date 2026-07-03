#pragma once

#include <cmath>

class EnvelopeShaper
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        setAttackTime(100.0f);
        setReleaseTime(100.0f);
    }

    void setAttackTime(float ms)
    {
        attack = std::exp(-1.0f / (ms * 0.001f * static_cast<float>(sampleRate)));
    }

    void setReleaseTime(float ms)
    {
        release = std::exp(-1.0f / (ms * 0.001f * static_cast<float>(sampleRate)));
    }

    float process(float input)
    {
        const float target = std::abs(input);
        const float coeff = (target > envelope) ? attack : release;
        envelope = target * (1.0f - coeff) + envelope * coeff;
        return input * envelope;
    }

private:
    double sampleRate = 44100.0;
    float attack = 0.0f;
    float release = 0.0f;
    float envelope = 0.0f;
};
