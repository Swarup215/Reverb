#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class PresetManager
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& apvtsIn)
        : apvts(apvtsIn)
    {
        auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile("ReverseFlow").getChildFile("Presets");
        if (!dir.exists())
            dir.createDirectory();
        presetDir = dir;
    }

    void savePreset(const juce::String& name)
    {
        auto file = presetDir.getChildFile(name + ".rfp");
        auto xml = apvts.copyState().createXml();
        if (xml != nullptr)
            file.replaceWithText(xml->toString());
    }

    void loadPreset(const juce::String& name)
    {
        auto file = presetDir.getChildFile(name + ".rfp");
        if (!file.existsAsFile())
            return;

        auto xml = juce::XmlDocument::parse(file);
        if (xml != nullptr)
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }

    juce::StringArray getPresetNames() const
    {
        juce::StringArray names;
        auto files = presetDir.findChildFiles(juce::File::findFiles, false, "*.rfp");
        for (auto& file : files)
            names.add(file.getFileNameWithoutExtension());
        return names;
    }

    void loadFactoryPreset(int index)
    {
        static const float presets[10][6] = {
            { 1, 0.5f, 1.0f, 2.0f, 1, 0.0f },
            { 2, 0.6f, 1.5f, 3.0f, 2, 0.1f },
            { 1, 0.5f, 0.8f, 1.5f, 1, 0.0f },
            { 3, 0.7f, 2.0f, 4.0f, 3, 0.2f },
            { 2, 0.6f, 1.2f, 2.5f, 2, 0.1f },
            { 3, 0.8f, 3.0f, 5.0f, 3, 0.3f },
            { 0, 0.4f, 0.3f, 0.8f, 0, 0.0f },
            { 0, 0.5f, 0.2f, 0.6f, 0, 0.0f },
            { 3, 0.7f, 2.5f, 6.0f, 3, -0.2f },
            { 3, 0.9f, 4.0f, 8.0f, 3, 0.0f },
        };

        if (index < 0 || index >= 10)
            return;

        auto* mode = apvts.getParameter("mode");
        auto* mix = apvts.getParameter("mix");
        auto* rev = apvts.getParameter("reverseTime");
        auto* dec = apvts.getParameter("decay");
        auto* size = apvts.getParameter("size");
        auto* tone = apvts.getParameter("tone");

        if (mode != nullptr) mode->setValueNotifyingHost(presets[index][0] / 4.0f);
        if (mix != nullptr) mix->setValueNotifyingHost(presets[index][1]);
        if (rev != nullptr) rev->setValueNotifyingHost((presets[index][2] - 0.1f) / 4.9f);
        if (dec != nullptr) dec->setValueNotifyingHost((presets[index][3] - 0.1f) / 9.9f);
        if (size != nullptr) size->setValueNotifyingHost(presets[index][4] / 3.0f);
        if (tone != nullptr) tone->setValueNotifyingHost((presets[index][5] + 1.0f) / 2.0f);
    }

    juce::StringArray getFactoryPresetNames() const
    {
        return {
            "Pop Vocal Lift", "Dream Pop", "Billie Style", "EDM Sweep", "Future Bass",
            "Choir Rise", "Trap Reverse", "Snare Pull", "Dark Hall", "Massive Build"
        };
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::File presetDir;
};
