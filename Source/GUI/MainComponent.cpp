#include "MainComponent.h"
#include "PluginProcessor.h"

MainComponent::MainComponent(ReverseFlowAudioProcessor& p)
    : processor(p),
      apvts(p.getAPVTS()),
      presetManager(apvts),
      modeSelector(apvts),
      mixKnob(apvts, Parameters::mix, "Mix"),
      revTimeKnob(apvts, Parameters::reverseTime, "Reverse Time"),
      decayKnob(apvts, Parameters::decay, "Decay"),
      sizeKnob(apvts, Parameters::size, "Size"),
      preDelayKnob(apvts, Parameters::preDelay, "Pre Delay"),
      widthKnob(apvts, Parameters::width, "Width"),
      toneKnob(apvts, Parameters::tone, "Tone"),
      duckKnob(apvts, Parameters::duck, "Duck"),
      sensKnob(apvts, Parameters::sensitivity, "Sensitivity"),
      outGainKnob(apvts, Parameters::outputGain, "Output"),
      inMeter("In"),
      outMeter("Out"),
      wetMeter("Wet")
{
    addAndMakeVisible(modeSelector);
    addAndMakeVisible(waveform);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(revTimeKnob);
    addAndMakeVisible(decayKnob);
    addAndMakeVisible(sizeKnob);
    addAndMakeVisible(preDelayKnob);
    addAndMakeVisible(widthKnob);
    addAndMakeVisible(toneKnob);
    addAndMakeVisible(duckKnob);
    addAndMakeVisible(sensKnob);
    addAndMakeVisible(outGainKnob);
    addAndMakeVisible(inMeter);
    addAndMakeVisible(outMeter);
    addAndMakeVisible(wetMeter);
    addAndMakeVisible(freezeBtn);

    freezeBtn.setClickingTogglesState(true);
    freezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "freeze", freezeBtn);

    addAndMakeVisible(presetBox);
    presetBox.addItemList(presetManager.getFactoryPresetNames(), 1);
    presetBox.onChange = [this]()
    {
        if (presetBox.getSelectedId() > 0)
            presetManager.loadFactoryPreset(presetBox.getSelectedId() - 1);
    };

    addAndMakeVisible(savePresetBtn);
    savePresetBtn.onClick = [this]()
    {
        const auto presetName = "UserPreset-" + juce::Time::getCurrentTime().formatted("%Y%m%d-%H%M%S");
        presetManager.savePreset(presetName);
    };

    addAndMakeVisible(loadPresetBtn);
    loadPresetBtn.onClick = [this]()
    {
        auto names = presetManager.getPresetNames();
        juce::PopupMenu menu;
        for (int i = 0; i < names.size(); ++i)
            menu.addItem(i + 1, names[i], true, false, nullptr);
        menu.showMenuAsync(juce::PopupMenu::Options(), [this, names](int result)
        {
            if (result > 0)
                presetManager.loadPreset(names[result - 1]);
        });
    };

    addAndMakeVisible(smartLabel);
    smartLabel.setJustificationType(juce::Justification::centred);
    smartLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFAAAAAA));
    smartLabel.setFont(juce::Font(11.0f));

    startTimerHz(30);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A1A));
}

void MainComponent::resized()
{
    auto b = getLocalBounds().reduced(10);

    auto top = b.removeFromTop(45);
    presetBox.setBounds(top.removeFromLeft(180).reduced(2));
    savePresetBtn.setBounds(top.removeFromLeft(80).reduced(2));
    loadPresetBtn.setBounds(top.removeFromLeft(80).reduced(2));
    modeSelector.setBounds(top.removeFromLeft(top.getWidth() - 150).reduced(2));
    smartLabel.setBounds(top);

    auto right = b.removeFromRight(50);
    inMeter.setBounds(right.removeFromTop(right.getHeight() / 3).reduced(2));
    wetMeter.setBounds(right.removeFromTop(right.getHeight() / 2).reduced(2));
    outMeter.setBounds(right.reduced(2));

    auto centre = b;
    waveform.setBounds(centre.removeFromTop(static_cast<int>(centre.getHeight() * 0.45f)).reduced(4));

    auto freezeRow = centre.removeFromTop(30);
    freezeBtn.setBounds(freezeRow.removeFromLeft(100).reduced(2));

    auto knobs = centre;
    const int kw = knobs.getWidth() / 5;

    mixKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    revTimeKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    decayKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    sizeKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    preDelayKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));

    widthKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    toneKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    duckKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    sensKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
    outGainKnob.setBounds(knobs.removeFromLeft(kw).reduced(4));
}

void MainComponent::timerCallback()
{
    inMeter.setLevel(processor.getInputLevel());
    outMeter.setLevel(processor.getOutputLevel());
    wetMeter.setLevel(processor.getWetLevel());

    if (apvts.getRawParameterValue("smartDetect")->load() > 0.5f)
        smartLabel.setText("Detected: " + processor.getSmartDetectorName(), juce::dontSendNotification);
    else
        smartLabel.setText("Smart Detect: Off", juce::dontSendNotification);
}
