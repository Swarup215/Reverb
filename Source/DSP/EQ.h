#pragma once

#include <juce_dsp/juce_dsp.h>

class EQ
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        for (int ch = 0; ch < 2; ++ch)
        {
            hpf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 20.0f);
            lpf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, 20000.0f);
            shelf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr, 3000.0f, 0.707f, 1.0f);
        }
    }

    void setHPF(float freq)
    {
        for (int ch = 0; ch < 2; ++ch)
            hpf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq);
    }

    void setLPF(float freq)
    {
        for (int ch = 0; ch < 2; ++ch)
            lpf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq);
    }

    void setTone(float tone)
    {
        const float gain = juce::Decibels::decibelsToGain(tone * 6.0f);
        for (int ch = 0; ch < 2; ++ch)
            shelf[ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 3000.0f, 0.707f, gain);
    }

    float processSample(float sample, int channel)
    {
        const int ch = juce::jlimit(0, 1, channel);
        sample = hpf[ch].processSample(sample);
        sample = lpf[ch].processSample(sample);
        sample = shelf[ch].processSample(sample);
        return sample;
    }

private:
    juce::dsp::IIR::Filter<float> hpf[2], lpf[2], shelf[2];
    double sampleRate = 44100.0;
};
