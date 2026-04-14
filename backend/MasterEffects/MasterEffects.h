/*
  ==============================================================================
    FILE: MasterEffects.h
    PROJECT: SONAR MIDI PLAYER
    VERSION: 2.1.0
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MasterEffects
{
public:
    MasterEffects();
    ~MasterEffects() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float> &buffer);

private:
    // Pomocný typ pro filtr (Mono verze duplikovaná pro Stereo)
    using FilterType = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using StereoFilter = juce::dsp::ProcessorDuplicator<FilterType, FilterCoefs>;

    // AI Efektový řetězec: 0: Low-Cut, 1: Compressor, 2: Limiter
    juce::dsp::ProcessorChain<StereoFilter, juce::dsp::Compressor<float>, juce::dsp::Limiter<float>> fxChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterEffects)
};