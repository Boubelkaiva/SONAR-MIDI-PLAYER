/*
  ==============================================================================
    FILE: TransportComponent.h
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SF2ListComponent/SF2ListComponent.h"

// AI: Dopředná deklarace, aby hlavička věděla, co je MidiPlayer
class MidiPlayer;

class TransportComponent : public juce::Component
{
public:
  // AI: Konstruktor teď přijímá referenci na tvůj engine
  TransportComponent(MidiPlayer &player);
  ~TransportComponent() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  std::function<void()> onStartClicked;
  std::function<void()> onStopClicked;
  std::function<void()> onPauseClicked;

  std::unique_ptr<SF2ListComponent> sf2List;

private:
  MidiPlayer &midiPlayer; // AI: Uložená reference na player

  juce::TextButton startButton{"Start"};
  juce::TextButton stopButton{"Stop"};
  juce::TextButton pauseButton{"Pause"};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};