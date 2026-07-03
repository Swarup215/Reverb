#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class Limiter
{
public:
    void prepare(double sr)
    {
        release = std::exp(-1.0f / (0.01f * static_cast<float>(sr)));
    }

    void setThreshold(float db) { threshold = juce::Decibels::decibelsToGain(db); }

    float process(float input)
    {
        const float env = std::abs(input);
        if (env > envelope)
            envelope = env;
        else
            envelope *= release;

        if (envelope > threshold && envelope > 0.0f)
            return input * (threshold / envelope);
        return input;
    }

private:
    float threshold = 1.0f;
    float envelope = 0.0f;
    float release = 0.99f;
};
