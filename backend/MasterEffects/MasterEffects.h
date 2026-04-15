#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

class MasterEffects
{
public:
  MasterEffects();
  ~MasterEffects() = default;

  void prepare(double sampleRate, int samplesPerBlock);
  void process(juce::AudioBuffer<float> &buffer);

  // --- VEŘEJNÉ METODY PRO OVLÁDÁNÍ Z FRONTENDU ---

  void setLowCutFrequency(float frequency)
  {
    auto &lowCut = fxChain.get<0>();
    // Přepočet koeficientů filtru za chodu (nutný sampleRate)
    *lowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, frequency);
  }

  void setCompThreshold(float thresholdDb)
  {
    fxChain.get<1>().setThreshold(thresholdDb);
  }

  void setCompRatio(float ratio)
  {
    fxChain.get<1>().setRatio(ratio);
  }

  void setLimiterThreshold(float thresholdDb)
  {
    fxChain.get<2>().setThreshold(thresholdDb);
  }

private:
  // Explicitní definice typů pro klid v duši editoru
  using Filter = juce::dsp::IIR::Filter<float>;
  using Coefficients = juce::dsp::IIR::Coefficients<float>;
  using StereoFilter = juce::dsp::ProcessorDuplicator<Filter, Coefficients>;

  // 0: HighPass, 1: Compressor (S-Type), 2: Limiter (MAX)
  juce::dsp::ProcessorChain<StereoFilter, juce::dsp::Compressor<float>, juce::dsp::Limiter<float>> fxChain;

  // Musíme si pamatovat sample rate pro pozdější přepočet filtru
  double currentSampleRate = 44100.0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterEffects)
};