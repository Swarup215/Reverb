#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

class GranularEngine
{
public:
    void prepare(double sr) { sampleRate = sr; }

    void freeze(const juce::AudioBuffer<float>& source, int startSample, int length)
    {
        const int safeStart = juce::jlimit(0, juce::jmax(0, source.getNumSamples() - 1), startSample);
        const int safeLength = juce::jlimit(1, juce::jmax(1, source.getNumSamples() - safeStart), juce::jmax(1, length));

        grainBuffer.setSize(2, safeLength);
        for (int ch = 0; ch < juce::jmin(source.getNumChannels(), 2); ++ch)
            grainBuffer.copyFrom(ch, 0, source, ch, safeStart, safeLength);
        if (source.getNumChannels() == 1)
            grainBuffer.copyFrom(1, 0, source, 0, safeStart, safeLength);

        grains.clear();
        grainCounter = 0;
        frozen = true;
    }

    void setDensity(float d) { density = d; }
    void setTexture(float t) { texture = t; }

    void process(float& left, float& right)
    {
        if (!frozen || grainBuffer.getNumSamples() == 0)
        {
            left = 0.0f;
            right = 0.0f;
            return;
        }

        ++grainCounter;
        const int spawnRate = juce::jmax(1, static_cast<int>(sampleRate / (10.0f + density * 90.0f)));
        if (grainCounter >= spawnRate)
        {
            grainCounter = 0;
            spawnGrain();
        }

        float outL = 0.0f;
        float outR = 0.0f;
        for (auto it = grains.begin(); it != grains.end();)
        {
            const float env = it->getEnvelope();
            if (env <= 0.0f)
            {
                it = grains.erase(it);
                continue;
            }

            const int pos = static_cast<int>(it->position);
            float s = 0.0f;
            if (pos >= 0 && pos < grainBuffer.getNumSamples())
                s = grainBuffer.getSample(0, pos) * 0.5f + grainBuffer.getSample(1, pos) * 0.5f;

            outL += s * env * (1.0f - it->pan);
            outR += s * env * it->pan;

            it->position += it->rate;
            ++it->age;
            ++it;
        }

        left = outL;
        right = outR;
    }

private:
    struct Grain
    {
        float position = 0.0f;
        float rate = 1.0f;
        float pan = 0.5f;
        int age = 0;
        int length = 1000;

        float getEnvelope() const
        {
            if (age < 100)
                return age / 100.0f;
            if (age > length - 100)
                return (length - age) / 100.0f;
            return 1.0f;
        }
    };

    void spawnGrain()
    {
        if (grainBuffer.getNumSamples() == 0)
            return;

        Grain g;
        g.position = static_cast<float>(std::rand() % grainBuffer.getNumSamples());
        g.rate = 0.5f + texture * 1.5f;
        g.pan = static_cast<float>(std::rand()) / RAND_MAX;
        g.length = juce::jmax(1, static_cast<int>(sampleRate * 0.1f));
        grains.push_back(g);
    }

    juce::AudioBuffer<float> grainBuffer;
    std::vector<Grain> grains;
    double sampleRate = 44100.0;
    float density = 0.5f;
    float texture = 0.5f;
    int grainCounter = 0;
    bool frozen = false;
};
