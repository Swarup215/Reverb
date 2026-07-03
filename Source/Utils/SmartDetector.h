#pragma once

#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>

class SmartDetector
{
public:
    void prepare(double sampleRateIn)
    {
        sampleRate = sampleRateIn;
        fft = std::make_unique<juce::dsp::FFT>(12);
        fftData.resize(4096 * 2);
    }

    void process(const juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0 || fft == nullptr)
            return;

        const int numSamples = juce::jmin(buffer.getNumSamples(), 4096);
        auto* data = buffer.getReadPointer(0);
        std::copy(data, data + numSamples, fftData.begin());
        std::fill(fftData.begin() + numSamples, fftData.begin() + 4096, 0.0f);

        fft->performFrequencyOnlyForwardTransform(fftData.data());

        float sum = 0.0f;
        float weightedSum = 0.0f;
        for (int i = 0; i < 2048; ++i)
        {
            sum += fftData[i];
            weightedSum += fftData[i] * static_cast<float>(i);
        }
        spectralCentroid = sum > 0.0f ? weightedSum / sum : 0.0f;

        zcr = 0.0f;
        for (int i = 1; i < numSamples; ++i)
            if ((data[i] >= 0.0f) != (data[i - 1] >= 0.0f))
                zcr++;
        zcr /= juce::jmax(1, numSamples);

        rms = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            rms += data[i] * data[i];
        rms = std::sqrt(rms / juce::jmax(1, numSamples));

        const float f[3] = { spectralCentroid / 1000.0f, zcr * 10.0f, rms * 10.0f };

        if (f[2] > 0.5f && f[1] > 0.3f) detected = 1;
        else if (f[0] > 1.5f && f[1] < 0.2f) detected = 3;
        else if (f[2] > 0.3f && f[0] > 1.0f) detected = 2;
        else if (f[2] < 0.2f && f[0] < 0.5f) detected = 4;
        else if (f[2] > 0.3f && f[1] > 0.15f) detected = 0;
        else if (f[2] < 0.3f && f[0] < 0.8f) detected = 6;
        else detected = 5;
    }

    int getDetectedSource() const { return detected; }

    juce::String getSourceName() const
    {
        const char* names[] = { "Vocal", "Drums", "Piano", "Strings", "Synth", "Guitar", "Bass" };
        return names[juce::jlimit(0, 6, detected)];
    }

private:
    std::unique_ptr<juce::dsp::FFT> fft;
    std::vector<float> fftData;
    double sampleRate = 44100.0;
    float spectralCentroid = 0.0f;
    float zcr = 0.0f;
    float rms = 0.0f;
    int detected = 0;
};
