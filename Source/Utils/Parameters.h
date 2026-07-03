#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Parameters
{
    #define DECLARE_ID(name) const juce::ParameterID name(#name, 1)

    DECLARE_ID(mode);
    DECLARE_ID(mix);
    DECLARE_ID(reverseTime);
    DECLARE_ID(decay);
    DECLARE_ID(size);
    DECLARE_ID(preDelay);
    DECLARE_ID(width);
    DECLARE_ID(tone);
    DECLARE_ID(duck);
    DECLARE_ID(sensitivity);
    DECLARE_ID(freeze);
    DECLARE_ID(outputGain);
    DECLARE_ID(hpf);
    DECLARE_ID(lpf);
    DECLARE_ID(stereoBalance);
    DECLARE_ID(modDepth);
    DECLARE_ID(modSpeed);
    DECLARE_ID(diffusion);
    DECLARE_ID(density);
    DECLARE_ID(oversampling);
    DECLARE_ID(tempoSync);
    DECLARE_ID(tempoSyncNote);
    DECLARE_ID(smartDetect);

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterChoice>(mode, "Mode",
            juce::StringArray { "Tight", "Vocal", "Dream", "Cinema", "Infinite" }, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(mix, "Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(reverseTime, "Reverse Time",
            juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(decay, "Decay",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 2.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>(size, "Size",
            juce::StringArray { "Small", "Medium", "Large", "Huge" }, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(preDelay, "Pre Delay",
            juce::NormalisableRange<float>(0.0f, 0.5f, 0.001f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(width, "Width",
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(tone, "Tone",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(duck, "Duck",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(sensitivity, "Sensitivity",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterBool>(freeze, "Freeze", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(outputGain, "Output Gain",
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(hpf, "HPF",
            juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f), 20.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(lpf, "LPF",
            juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f), 20000.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(stereoBalance, "Stereo Balance",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(modDepth, "Mod Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(modSpeed, "Mod Speed",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(diffusion, "Diffusion",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(density, "Density",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterChoice>(oversampling, "Oversampling",
            juce::StringArray { "Off", "2x", "4x", "8x" }, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(tempoSync, "Tempo Sync", false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(tempoSyncNote, "Tempo Sync Note",
            juce::StringArray { "1/64", "1/32", "1/16", "1/8", "1/4", "1/2", "1 Bar", "2 Bars",
                               "1/16 Dotted", "1/8 Dotted", "1/4 Dotted", "1/8 Triplet", "1/4 Triplet" }, 4));
        layout.add(std::make_unique<juce::AudioParameterBool>(smartDetect, "Smart Detect", false));

        return layout;
    }
}
#undef DECLARE_ID
