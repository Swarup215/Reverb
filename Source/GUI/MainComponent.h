#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Knob.h"
#include "WaveformDisplay.h"
#include "ModeSelector.h"
#include "Meter.h"
#include "Utils/Parameters.h"
#include "Utils/PresetManager.h"

class ReverseFlowAudioProcessor;

class MainComponent : public juce::Component, public juce::Timer
{
public:
    explicit MainComponent(ReverseFlowAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    ReverseFlowAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    PresetManager presetManager;

    juce::ComboBox presetBox;
    juce::TextButton savePresetBtn { "Save" }, loadPresetBtn { "Load" };
    ModeSelector modeSelector;
    WaveformDisplay waveform;

    Knob mixKnob, revTimeKnob, decayKnob, sizeKnob;
    Knob preDelayKnob, widthKnob, toneKnob, duckKnob;
    Knob sensKnob, outGainKnob;

    Meter inMeter, outMeter, wetMeter;
    juce::Label smartLabel;
    juce::TextButton freezeBtn { "Freeze" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttachment;
};
