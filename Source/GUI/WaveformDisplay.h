#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class WaveformDisplay : public juce::Component, public juce::Timer
{
public:
    WaveformDisplay() : buffer(2, 1024)
    {
        buffer.clear();
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF1A1A1A));

        auto b = getLocalBounds().reduced(4);
        auto h = b.getHeight() / 2;

        g.setColour(juce::Colour(0xFF4A4A4A));
        drawChannel(g, b.removeFromTop(h), 0);

        g.setColour(juce::Colour(0xFF6B4EE6));
        drawChannel(g, b, 1);
    }

    void timerCallback() override { repaint(); }

    void pushSamples(float left, float right, float wetL, float wetR)
    {
        buffer.setSample(0, writePos, (left + right) * 0.5f);
        buffer.setSample(1, writePos, (wetL + wetR) * 0.5f);
        writePos = (writePos + 1) % buffer.getNumSamples();
    }

private:
    void drawChannel(juce::Graphics& g, juce::Rectangle<int> bounds, int channel)
    {
        juce::Path p;
        const int w = bounds.getWidth();
        const float cy = bounds.getCentreY();

        p.startNewSubPath(bounds.getX(), cy);
        for (int i = 0; i < w; ++i)
        {
            const int idx = (writePos + i) % buffer.getNumSamples();
            const float s = buffer.getSample(channel, idx);
            p.lineTo(bounds.getX() + i, cy - s * bounds.getHeight() * 0.4f);
        }
        g.strokePath(p, juce::PathStrokeType(1.5f));
    }

    juce::AudioBuffer<float> buffer;
    int writePos = 0;
};
