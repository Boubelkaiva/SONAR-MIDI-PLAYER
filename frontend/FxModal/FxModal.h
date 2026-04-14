/*
  ==============================================================================
    FILE: FxModal.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Clean header with rounded corners declaration.
    UPDATED: Přidána metoda setInitialValues pro synchronizaci s originál MIDI.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FxModal : public juce::Component
{
public:
  struct Listener
  {
    std::function<void(int)> onPanChanged;
    std::function<void(int)> onReverbChanged;
    std::function<void(int)> onChorusChanged;
    std::function<void()> onClose;
  };

  FxModal(int trackNumber, Listener listener);
  ~FxModal() override;

  /** * Nastaví pozice sliderů podle dat načtených z MIDI souboru.
   * Volá se hned po vytvoření okna v MainComponent.
   */
  void setInitialValues(int pan, int reverb, int chorus);

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  int trackNum;
  Listener listener;

  juce::Slider panSlider;
  juce::Slider reverbSlider;
  juce::Slider chorusSlider;

  juce::Label panLabel, reverbLabel, chorusLabel;
  juce::Label title;
  juce::TextButton closeButton;

  void setupSlider(juce::Slider &s, juce::Label &l, const juce::String &name);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxModal)
};