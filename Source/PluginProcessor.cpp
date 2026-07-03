#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils/TempoSync.h"

ReverseFlowAudioProcessor::ReverseFlowAudioProcessor()
    : apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout()),
      mixSmooth(0.5f),
      outGainSmooth(0.0f)
{
}

void ReverseFlowAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    int numChannels = getTotalNumInputChannels();
    if (numChannels == 0)
        numChannels = 2;

    reverseBuffer.prepare(numChannels, sr, 5.0, 10.0);
    reverbEngine.prepare(sr, numChannels);
    envelopeShaper.prepare(sr);
    transientDetector.prepare(sr);
    granularEngine.prepare(sr);
    eq.prepare(sr);
    limiter.prepare(sr);
    smartDetector.prepare(sr);

    juce::dsp::ProcessSpec spec { sr, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(numChannels) };
    const int osChoice = static_cast<int>(apvts.getRawParameterValue("oversampling")->load());
    const int osFactor = 1 << osChoice;
    oversampler.prepare(spec, osFactor);

    mixSmooth.reset(sr, 0.05);
    outGainSmooth.reset(sr, 0.05);

    updateParameters();

    latencySamples = calculateLatency();
    setLatencySamples(latencySamples + oversampler.getLatencySamples());
    reverseBuffer.setReverseReadOffset(static_cast<juce::uint32>(latencySamples));
}

void ReverseFlowAudioProcessor::updateParameters()
{
    const float mix = apvts.getRawParameterValue("mix")->load();
    const float outGain = apvts.getRawParameterValue("outputGain")->load();
    mixSmooth.setTargetValue(mix);
    outGainSmooth.setTargetValue(outGain);

    const float decay = apvts.getRawParameterValue("decay")->load();
    const float sizeVal = apvts.getRawParameterValue("size")->load() / 3.0f;
    const float diffusion = apvts.getRawParameterValue("diffusion")->load();
    const float density = apvts.getRawParameterValue("density")->load();
    const float tone = apvts.getRawParameterValue("tone")->load();
    const float damping = 1.0f - tone * 0.5f + 0.5f;

    const int mode = static_cast<int>(apvts.getRawParameterValue("mode")->load() * 4.0f);
    static const float sizeScale[5] = { 0.3f, 0.6f, 0.8f, 1.0f, 1.2f };
    static const float diffScale[5] = { 0.4f, 0.7f, 0.8f, 0.9f, 1.0f };
    static const float densScale[5] = { 0.5f, 0.7f, 0.6f, 0.8f, 1.0f };

    reverbEngine.setParameters(sizeVal * sizeScale[mode], decay, diffusion * diffScale[mode], density * densScale[mode], damping);

    const float attackMs = 10.0f + mode * 50.0f;
    envelopeShaper.setAttackTime(attackMs);
    envelopeShaper.setReleaseTime(attackMs * 0.5f);

    const float hpf = apvts.getRawParameterValue("hpf")->load();
    const float lpf = apvts.getRawParameterValue("lpf")->load();
    eq.setHPF(hpf);
    eq.setLPF(lpf);
    eq.setTone(tone);

    transientDetector.setSensitivity(apvts.getRawParameterValue("sensitivity")->load());
    saturation.setDrive(mode == 3 ? 0.3f : 0.0f);
    limiter.setThreshold(-0.5f);
}

int ReverseFlowAudioProcessor::calculateLatency() const
{
    float revTime = apvts.getRawParameterValue("reverseTime")->load();
    const float preDelay = apvts.getRawParameterValue("preDelay")->load();

    const bool tempoSync = apvts.getRawParameterValue("tempoSync")->load() > 0.5f;
    if (tempoSync && getPlayHead() != nullptr)
    {
        juce::AudioPlayHead::CurrentPositionInfo pos;
        if (getPlayHead()->getCurrentPosition(pos))
        {
            const int noteIdx = static_cast<int>(apvts.getRawParameterValue("tempoSyncNote")->load() * 12.0f);
            revTime = TempoSync::noteToSeconds(static_cast<TempoSync::Note>(noteIdx), pos.bpm);
        }
    }

    return static_cast<int>((revTime + preDelay) * sampleRate);
}

void ReverseFlowAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numIn = getTotalNumInputChannels();
    const int numOut = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = numIn; i < numOut; ++i)
        buffer.clear(i, 0, numSamples);

    updateParameters();

    const int newLatency = calculateLatency();
    if (newLatency != latencySamples)
    {
        latencySamples = newLatency;
        setLatencySamples(latencySamples + oversampler.getLatencySamples());
        reverseBuffer.setReverseReadOffset(static_cast<juce::uint32>(latencySamples));
    }

    const bool freeze = apvts.getRawParameterValue("freeze")->load() > 0.5f;
    if (freeze && !freezePrev)
        granularEngine.freeze(reverseBuffer.getInternalBuffer(), 0, latencySamples);
    freezePrev = freeze;

    const bool smart = apvts.getRawParameterValue("smartDetect")->load() > 0.5f;
    if (smart && numSamples >= 4096)
        smartDetector.process(buffer);

    const float width = apvts.getRawParameterValue("width")->load();
    const float duck = apvts.getRawParameterValue("duck")->load();
    const int mode = static_cast<int>(apvts.getRawParameterValue("mode")->load() * 4.0f);

    float inLvl = 0.0f;
    float outLvl = 0.0f;
    float wetLvl = 0.0f;

    for (int s = 0; s < numSamples; ++s)
    {
        const float inL = numIn > 0 ? buffer.getReadPointer(0)[s] : 0.0f;
        const float inR = numIn > 1 ? buffer.getReadPointer(1)[s] : inL;

        inLvl = std::max(inLvl, std::max(std::abs(inL), std::abs(inR)));

        reverseBuffer.writeSample(0, inL);
        reverseBuffer.writeSample(1, inR);
        reverseBuffer.advanceWriteIndex();

        const float dryL = reverseBuffer.readDry(0, static_cast<juce::uint32>(latencySamples));
        const float dryR = reverseBuffer.readDry(1, static_cast<juce::uint32>(latencySamples));

        const float revL = reverseBuffer.readReverse(0);
        const float revR = reverseBuffer.readReverse(1);
        reverseBuffer.advanceReverseRead();

        const bool transient = transientDetector.process(inL);
        const float duckGain = transient ? (1.0f - duck) : 1.0f;

        float wetL = 0.0f;
        float wetR = 0.0f;

        if (mode == 4 && freeze)
        {
            granularEngine.process(wetL, wetR);
        }
        else
        {
            reverbEngine.process(revL, revR, wetL, wetR);
            wetL = envelopeShaper.process(wetL);
            wetR = envelopeShaper.process(wetR);
        }

        wetL *= duckGain;
        wetR *= duckGain;

        const float mid = (wetL + wetR) * 0.5f;
        float side = (wetL - wetR) * 0.5f;
        side *= width;
        wetL = mid + side;
        wetR = mid - side;

        wetL = eq.processSample(wetL, 0);
        wetR = eq.processSample(wetR, 1);

        wetL = saturation.process(wetL);
        wetR = saturation.process(wetR);

        wetLvl = std::max(wetLvl, std::max(std::abs(wetL), std::abs(wetR)));

        const float mix = mixSmooth.getNextValue();
        const float gain = juce::Decibels::decibelsToGain(outGainSmooth.getNextValue());

        float outL = dryL * (1.0f - mix) + wetL * mix;
        float outR = dryR * (1.0f - mix) + wetR * mix;

        outL *= gain;
        outR *= gain;

        outL = limiter.process(outL);
        outR = limiter.process(outR);

        buffer.setSample(0, s, outL);
        if (numOut > 1)
            buffer.setSample(1, s, outR);

        outLvl = std::max(outLvl, std::max(std::abs(outL), std::abs(outR)));
    }

    inputLevel.store(inLvl);
    outputLevel.store(outLvl);
    wetLevel.store(wetLvl);
}

void ReverseFlowAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverseFlowAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* ReverseFlowAudioProcessor::createEditor()
{
    return new ReverseFlowAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverseFlowAudioProcessor();
}
