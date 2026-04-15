/*
  ==============================================================================
    FILE: MainComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main container - handling audio and UI layout.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Frontend komponenty
#include "../TrackPanelComponent/TrackPanelComponent.h"
#include "../TransportComponent/TransportComponent.h"
#include "../SF2ListComponent/SF2ListComponent.h"
#include "../MasterPanel/MasterPanel.h"
// AI FIX: Přidán include pro tvou komponentu verze
#include "../VersionFooter/VersionFooter.h"

// Backend moduly
#include "../../backend/MidiPlayer/MidiPlayer.h"
#include "../../backend/BankManager/BankManager.h"
#include "../../backend/MidiAnalyzer/MidiAnalyzer.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::ChangeListener
{
public:
  MainComponent();
  ~MainComponent() override;

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  void changeListenerCallback(juce::ChangeBroadcaster *source) override;
  void parentHierarchyChanged() override;

private:
  // Backend
  std::unique_ptr<MidiPlayer> midiPlayer;
  std::unique_ptr<BankManager> bankManager;

  // Frontend
  std::unique_ptr<TrackPanelComponent> trackPanel;
  std::unique_ptr<TransportComponent> transport;
  std::unique_ptr<MasterPanel> masterPanel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};