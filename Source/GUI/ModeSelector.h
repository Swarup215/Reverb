#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ModeSelector : public juce::Component, public juce::AudioProcessorValueTreeState::Listener
{
public:
    ModeSelector(juce::AudioProcessorValueTreeState& apvtsIn) : apvts(apvtsIn)
    {
        juce::StringArray names = { "Tight", "Vocal", "Dream", "Cinema", "Infinite" };
        for (int i = 0; i < names.size(); ++i)
        {
            auto btn = std::make_unique<juce::TextButton>(names[i]);
            btn->setRadioGroupId(1);
            btn->setClickingTogglesState(true);
            btn->onClick = [this, i]()
            {
                if (auto* p = apvts.getParameter("mode"))
                    p->setValueNotifyingHost(i / 4.0f);
            };
            addAndMakeVisible(btn.get());
            buttons.push_back(std::move(btn));
        }
        apvts.addParameterListener("mode", this);
        updateState();
    }

    ~ModeSelector() override
    {
        apvts.removeParameterListener("mode", this);
    }

    void parameterChanged(const juce::String& id, float) override
    {
        if (id == "mode")
            juce::MessageManager::callAsync([this]() { updateState(); });
    }

    void updateState()
    {
        if (auto* p = apvts.getParameter("mode"))
        {
            const int idx = static_cast<int>(p->getValue() * 4.0f);
            for (int i = 0; i < buttons.size(); ++i)
                buttons[i]->setToggleState(i == idx, juce::dontSendNotification);
        }
    }

    void resized() override
    {
        auto b = getLocalBounds();
        const int w = b.getWidth() / juce::jmax(1, static_cast<int>(buttons.size()));
        for (auto& btn : buttons)
            btn->setBounds(b.removeFromLeft(w).reduced(2, 4));
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<std::unique_ptr<juce::TextButton>> buttons;
};
