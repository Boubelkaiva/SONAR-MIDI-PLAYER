/*
  ==============================================================================
    FILE: TransportComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main transport, Settings Bar, SF2 List and Version footer.
    VERSION: 2.1.2
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../SF2ListComponent/SF2ListComponent.h"
#include "../VersionFooter/VersionFooter.h"
#include "../../backend/MidiPlayer/MidiPlayer.h"

class SettingsBarComponent : public juce::Component
{
public:
  SettingsBarComponent(MidiPlayer &player);
  ~SettingsBarComponent() override = default;
  void paint(juce::Graphics &g) override {}
  void resized() override;

private:
  MidiPlayer &midiPlayer;
  juce::TextButton fxButton{"Master FX"}, settingsButton{"Audio HW"};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsBarComponent)
};

class TransportComponent : public juce::Component
{
public:
  TransportComponent(MidiPlayer &player, BankManager &manager);
  ~TransportComponent() override;
  void paint(juce::Graphics &g) override;
  void resized() override;

  std::function<void()> onStartClicked, onStopClicked, onPauseClicked;
  std::unique_ptr<SF2ListComponent> sf2List;

private:
  MidiPlayer &midiPlayer;
  juce::TextButton startButton{"Start"}, stopButton{"Stop"}, pauseButton{"Pause"};
  std::unique_ptr<SettingsBarComponent> settingsBar;
  std::unique_ptr<VersionFooter> versionFooter;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};