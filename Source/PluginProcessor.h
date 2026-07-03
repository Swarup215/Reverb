#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Utils/Parameters.h"
#include "DSP/ReverseBuffer.h"
#include "DSP/ReverbEngine.h"
#include "DSP/EnvelopeShaper.h"
#include "DSP/TransientDetector.h"
#include "DSP/GranularEngine.h"
#include "DSP/EQ.h"
#include "DSP/Saturation.h"
#include "DSP/Limiter.h"
#include "DSP/Oversampler.h"
#include "Utils/SmartDetector.h"

class ReverseFlowAudioProcessor : public juce::AudioProcessor
{
public:
    ReverseFlowAudioProcessor();
    ~ReverseFlowAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override {}

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ReverseFlow"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    juce::String getSmartDetectorName() const { return smartDetector.getSourceName(); }

    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }
    float getWetLevel() const { return wetLevel.load(); }

private:
    juce::AudioProcessorValueTreeState apvts;

    ReverseBuffer reverseBuffer;
    ReverbEngine reverbEngine;
    EnvelopeShaper envelopeShaper;
    TransientDetector transientDetector;
    GranularEngine granularEngine;
    EQ eq;
    Saturation saturation;
    Limiter limiter;
    Oversampler oversampler;
    SmartDetector smartDetector;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmooth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outGainSmooth;

    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };
    std::atomic<float> wetLevel { 0.0f };

    double sampleRate = 44100.0;
    int latencySamples = 0;
    bool freezePrev = false;

    void updateParameters();
    int calculateLatency() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverseFlowAudioProcessor)
};
