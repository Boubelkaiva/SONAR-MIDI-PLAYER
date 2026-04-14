/*
  ==============================================================================
    FILE: MasterEffects.cpp
    PROJECT: SONAR MIDI PLAYER
  ==============================================================================
*/

#include "MasterEffects.h"

MasterEffects::MasterEffects()
{
    // 1. Nastavení Kompresoru (S-Type styl)
    auto &compressor = fxChain.get<1>();
    compressor.setRatio(4.0f);
    compressor.setAttack(5.0f);
    compressor.setRelease(100.0f);
    compressor.setThreshold(-10.0f);

    // 2. Nastavení Limiteru (MAX styl)
    auto &limiter = fxChain.get<2>();
    limiter.setThreshold(-0.1f);
    limiter.setRelease(100.0f);
}

void MasterEffects::prepare(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 2};

    // 3. Nastavení Low-Cut filtru na 40Hz (pro tvoje 2000W bedny)
    auto &lowCut = fxChain.get<0>();
    *lowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 40.0f);

    fxChain.prepare(spec);
}

void MasterEffects::process(juce::AudioBuffer<float> &buffer)
{
    // Vytvoření kontextu pro zpracování
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Spuštění celého AI efektového řetězce
    fxChain.process(context);
}