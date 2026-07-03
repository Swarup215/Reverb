#include "PluginEditor.h"

ReverseFlowAudioProcessorEditor::ReverseFlowAudioProcessorEditor(ReverseFlowAudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      processor(p),
      mainComponent(p)
{
    setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(mainComponent);
    setSize(1000, 650);
    setResizable(true, true);
    setResizeLimits(700, 450, 1600, 1000);
    getConstrainer()->setFixedAspectRatio(1000.0 / 650.0);
}

void ReverseFlowAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A1A));
}

void ReverseFlowAudioProcessorEditor::resized()
{
    mainComponent.setBounds(getLocalBounds());
}
