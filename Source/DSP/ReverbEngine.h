#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <vector>

class ReverbEngine
{
public:
    void prepare(double sampleRateIn, int)
    {
        sampleRate = sampleRateIn;
        const double scale = sampleRate / 44100.0;

        const int lc[4] = { 1117, 1187, 1277, 1357 };
        const int rc[4] = { 1133, 1201, 1291, 1373 };
        const int lap[2] = { 223, 397 };
        const int rap[2] = { 211, 401 };

        for (int i = 0; i < 4; ++i)
        {
            leftCombs[i].prepare(juce::jmax(1, static_cast<int>(lc[i] * scale)));
            rightCombs[i].prepare(juce::jmax(1, static_cast<int>(rc[i] * scale)));
        }

        for (int i = 0; i < 2; ++i)
        {
            leftAllpass[i].prepare(juce::jmax(1, static_cast<int>(lap[i] * scale)));
            rightAllpass[i].prepare(juce::jmax(1, static_cast<int>(rap[i] * scale)));
        }

        reset();
    }

    void reset()
    {
        for (auto& comb : leftCombs) comb.reset();
        for (auto& comb : rightCombs) comb.reset();
        for (auto& ap : leftAllpass) ap.reset();
        for (auto& ap : rightAllpass) ap.reset();
    }

    void setParameters(float sizeIn, float decayIn, float diffusionIn, float densityIn, float damping)
    {
        size = sizeIn;
        decay = decayIn;
        diffusion = diffusionIn;
        density = densityIn;

        const double scale = sampleRate / 44100.0;
        const int lc[4] = { 1117, 1187, 1277, 1357 };
        const int rc[4] = { 1133, 1201, 1291, 1373 };

        for (int i = 0; i < 4; ++i)
        {
            int ds = juce::jmax(1, static_cast<int>(lc[i] * scale * (0.5f + size * 1.5f)));
            float dSec = ds / static_cast<float>(sampleRate);
            float fb = std::pow(10.0f, -3.0f * dSec / std::max(0.01f, decay));
            leftCombs[i].setDelay(ds);
            leftCombs[i].setFeedback(fb);
            leftCombs[i].setDamping(damping);

            ds = juce::jmax(1, static_cast<int>(rc[i] * scale * (0.5f + size * 1.5f)));
            dSec = ds / static_cast<float>(sampleRate);
            fb = std::pow(10.0f, -3.0f * dSec / std::max(0.01f, decay));
            rightCombs[i].setDelay(ds);
            rightCombs[i].setFeedback(fb);
            rightCombs[i].setDamping(damping);
        }

        const float apCoeff = 0.5f + diffusion * 0.3f;
        for (auto& ap : leftAllpass) ap.setCoefficient(apCoeff);
        for (auto& ap : rightAllpass) ap.setCoefficient(apCoeff);
    }

    void process(float inL, float inR, float& outL, float& outR)
    {
        const int numCombs = juce::jlimit(1, 4, 2 + static_cast<int>(density * 2.0f));

        float combL = 0.0f;
        float combR = 0.0f;
        for (int i = 0; i < numCombs; ++i)
        {
            combL += leftCombs[i].process(inL);
            combR += rightCombs[i].process(inR);
        }
        combL /= static_cast<float>(numCombs);
        combR /= static_cast<float>(numCombs);

        float apL = combL;
        float apR = combR;
        for (auto& ap : leftAllpass) apL = ap.process(apL);
        for (auto& ap : rightAllpass) apR = ap.process(apR);

        outL = apL;
        outR = apR;
    }

private:
    struct Comb
    {
        void prepare(int delaySamples)
        {
            delay = juce::jmax(1, delaySamples);
            buffer.assign(static_cast<size_t>(delay), 0.0f);
            write = 0;
        }

        void reset()
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            write = 0;
            filterState = 0.0f;
        }

        void setDelay(int d)
        {
            if (d != delay)
            {
                delay = juce::jmax(1, d);
                buffer.assign(static_cast<size_t>(delay), 0.0f);
                write = 0;
                filterState = 0.0f;
            }
        }

        void setFeedback(float f) { fb = f; }
        void setDamping(float d) { damp = d; }

        float process(float input)
        {
            const float out = buffer[static_cast<size_t>(write)];
            filterState = out * (1.0f - damp) + filterState * damp;
            buffer[static_cast<size_t>(write)] = input + filterState * fb;
            write = (write + 1) % delay;
            return out;
        }

        std::vector<float> buffer;
        int delay = 1000;
        int write = 0;
        float fb = 0.5f;
        float damp = 0.5f;
        float filterState = 0.0f;
    };

    struct Allpass
    {
        void prepare(int delaySamples)
        {
            delay = juce::jmax(1, delaySamples);
            buffer.assign(static_cast<size_t>(delay), 0.0f);
            write = 0;
        }

        void reset()
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            write = 0;
        }

        void setCoefficient(float c) { coeff = c; }

        float process(float input)
        {
            const float delayOut = buffer[static_cast<size_t>(write)];
            const float feedBack = input + delayOut * coeff;
            buffer[static_cast<size_t>(write)] = feedBack;
            write = (write + 1) % delay;
            return -input * coeff + delayOut;
        }

        std::vector<float> buffer;
        int delay = 100;
        int write = 0;
        float coeff = 0.5f;
    };

    std::array<Comb, 4> leftCombs, rightCombs;
    std::array<Allpass, 2> leftAllpass, rightAllpass;
    double sampleRate = 44100.0;
    float size = 1.0f;
    float decay = 1.0f;
    float diffusion = 0.5f;
    float density = 0.5f;
};
