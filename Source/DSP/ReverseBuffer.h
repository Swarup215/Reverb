#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class ReverseBuffer
{
public:
    void prepare(int numChannels, double sampleRateIn, double maxReverseTime, double maxDecay)
    {
        channels = juce::jmax(1, numChannels);
        const int maxLatency = static_cast<int>((maxReverseTime + maxDecay + 1.0) * sampleRateIn);
        bufferSize = juce::nextPowerOfTwo(static_cast<juce::uint32>(juce::jmax(1, maxLatency)));
        mask = bufferSize - 1;

        buffer.setSize(channels, static_cast<int>(bufferSize));
        buffer.clear();

        writeIndex = 0;
        reverseReadIndex = 0;
    }

    void reset()
    {
        buffer.clear();
        writeIndex = 0;
        reverseReadIndex = 0;
    }

    void writeSample(int channel, float sample)
    {
        buffer.setSample(channel, static_cast<int>(writeIndex & mask), sample);
    }

    void advanceWriteIndex()
    {
        ++writeIndex;
    }

    float readDry(int channel, uint32_t latency) const
    {
        const uint32_t idx = (writeIndex - latency) & mask;
        return buffer.getSample(channel, static_cast<int>(idx));
    }

    float readReverse(int channel) const
    {
        const uint32_t idx = reverseReadIndex & mask;
        return buffer.getSample(channel, static_cast<int>(idx));
    }

    void advanceReverseRead()
    {
        --reverseReadIndex;
    }

    void setReverseReadOffset(uint32_t latency)
    {
        reverseReadIndex = writeIndex - latency;
    }

    const juce::AudioBuffer<float>& getInternalBuffer() const { return buffer; }

private:
    juce::AudioBuffer<float> buffer;
    int channels = 0;
    juce::uint32 bufferSize = 0;
    juce::uint32 mask = 0;
    juce::uint32 writeIndex = 0;
    juce::uint32 reverseReadIndex = 0;
};
