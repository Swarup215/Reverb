#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class Knob : public juce::Component
{
public:
    Knob(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, const juce::String& labelText)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);

        valueLabel.setJustificationType(juce::Justification::centred);
        valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFAAAAAA));
        addAndMakeVisible(valueLabel);

        slider.onValueChange = [this]()
        {
            valueLabel.setText(slider.getTextFromValue(slider.getValue()), juce::dontSendNotification);
        };

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, id.getParamID(), slider);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        label.setBounds(b.removeFromTop(20));
        valueLabel.setBounds(b.removeFromBottom(16));
        slider.setBounds(b);
    }

    juce::Slider& getSlider() { return slider; }

private:
    juce::Slider slider;
    juce::Label label, valueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};
