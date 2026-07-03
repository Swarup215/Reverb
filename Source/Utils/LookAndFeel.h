#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ReverseFlowLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ReverseFlowLookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF1A1A1A));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF6B4EE6));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF6B4EE6));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF3A3A3A));
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF242424));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF6B4EE6));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF242424));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF242424));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override
    {
        auto radius = juce::jmin(width / 2.0f, height / 2.0f) - 6.0f;
        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                                    rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xFF3A3A3A));
        g.strokePath(backgroundArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                               rotaryStartAngle, angle, true);
        g.setColour(juce::Colour(0xFF6B4EE6));
        g.strokePath(valueArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        auto thumbLength = radius * 0.75f;
        auto thumbX = centreX + thumbLength * std::cos(angle - juce::MathConstants<float>::halfPi);
        auto thumbY = centreY + thumbLength * std::sin(angle - juce::MathConstants<float>::halfPi);
        g.setColour(juce::Colours::white);
        g.fillEllipse(thumbX - 5.0f, thumbY - 5.0f, 10.0f, 10.0f);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour&, bool, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        constexpr float cornerSize = 6.0f;

        g.setColour(button.getToggleState() ? juce::Colour(0xFF6B4EE6) : juce::Colour(0xFF242424));
        g.fillRoundedRectangle(bounds, cornerSize);

        if (button.isMouseOver() && !button.getToggleState())
        {
            g.setColour(juce::Colour(0xFF4A4A4A));
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(12.0f);
    }
};
