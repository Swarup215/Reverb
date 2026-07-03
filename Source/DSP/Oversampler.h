#pragma once

#include <juce_dsp/juce_dsp.h>

class Oversampler
{
public:
    void prepare(const juce::dsp::ProcessSpec&, int factorIn)
    {
        factor = juce::jmax(1, factorIn);
    }

    void reset() {}

    int getLatencySamples() const { return 0; }

    juce::dsp::AudioBlock<float> processUp(juce::AudioBuffer<float>& input)
    {
        return juce::dsp::AudioBlock<float>(input);
    }

    void processDown(juce::dsp::AudioBlock<float>& upBlock, juce::AudioBuffer<float>& output)
    {
        const auto numChannels = juce::jmin(output.getNumChannels(), static_cast<int>(upBlock.getNumChannels()));
        const auto numSamples = juce::jmin(output.getNumSamples(), static_cast<int>(upBlock.getNumSamples()));

        for (int ch = 0; ch < numChannels; ++ch)
            output.copyFrom(ch, 0, upBlock.getChannelPointer(static_cast<size_t>(ch)), numSamples);
    }

private:
    int factor = 1;
};
