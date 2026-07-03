#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Meter : public juce::Component, public juce::Timer
{
public:
    explicit Meter(const juce::String& n) : name(n)
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().reduced(2);
        g.setColour(juce::Colour(0xFF2A2A2A));
        g.fillRoundedRectangle(b.toFloat(), 4.0f);

        const float db = juce::Decibels::gainToDecibels(level);
        const float h = b.getHeight() * juce::jlimit(0.0f, 1.0f, (db + 60.0f) / 60.0f);

        g.setColour(db > -6.0f ? juce::Colour(0xFFFF4444)
                               : (db > -18.0f ? juce::Colour(0xFFFFAA44)
                                              : juce::Colour(0xFF44FF44)));
        g.fillRoundedRectangle(b.removeFromBottom(static_cast<int>(h)).toFloat(), 4.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(10.0f));
        g.drawText(name, b, juce::Justification::centredTop);
    }

    void timerCallback() override
    {
        level *= 0.9f;
        repaint();
    }

    void setLevel(float l) { level = std::max(level, l); }

private:
    juce::String name;
    float level = 0.0f;
};
