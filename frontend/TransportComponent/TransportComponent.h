/*
  ==============================================================================
    FILE: TransportComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Transport controls + Audio Settings.
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SF2ListComponent/SF2ListComponent.h"

// Dopředná deklarace
class MidiPlayer;

class TransportComponent : public juce::Component
{
public:
  TransportComponent(MidiPlayer &player);
  ~TransportComponent() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  // Callbacky pro propojení s hlavní logikou v MainComponent
  std::function<void()> onStartClicked;
  std::function<void()> onStopClicked;
  std::function<void()> onPauseClicked;

  std::unique_ptr<SF2ListComponent> sf2List;

private:
  MidiPlayer &midiPlayer;

  juce::TextButton startButton{"Start"};
  juce::TextButton stopButton{"Stop"};
  juce::TextButton pauseButton{"Pause"};

  // Nové tlačítko pro vyvolání Modal okna s nastavením HW
  juce::TextButton settingsButton{"Audio HW"};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};