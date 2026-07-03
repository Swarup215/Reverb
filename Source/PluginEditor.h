#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "GUI/MainComponent.h"
#include "Utils/LookAndFeel.h"

class ReverseFlowAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ReverseFlowAudioProcessorEditor(ReverseFlowAudioProcessor&);
    ~ReverseFlowAudioProcessorEditor() override { setLookAndFeel(nullptr); }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ReverseFlowAudioProcessor& processor;
    MainComponent mainComponent;
    ReverseFlowLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverseFlowAudioProcessorEditor)
};
