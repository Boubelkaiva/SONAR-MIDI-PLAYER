/*
  ==============================================================================

    FILE: MainComponent.h
    PROJECT: SONAR MIDI PLAYER
    MODULE: frontend/MainComponent
    DESCRIPTION: Header for the main container.
                 FIXED: Modular include paths and BankManager integration.
                 UPDATED: Added MidiAnalyzer for metadata extraction.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// AI: Frontend komponenty
#include "../TrackPanelComponent/TrackPanelComponent.h"
#include "../TransportComponent/TransportComponent.h"
#include "../SF2ListComponent/SF2ListComponent.h"
#include "../MasterPanel/MasterPanel.h"

// AI: Backend moduly
#include "../../backend/MidiPlayer/MidiPlayer.h"
#include "../../backend/BankManager/BankManager.h"
#include "../../backend/MidiAnalyzer/MidiAnalyzer.h" // AI: Nový modul pro analýzu souborů

class MainComponent : public juce::AudioAppComponent
{
public:
  MainComponent();
  ~MainComponent() override;

  // --- Audio metody ---
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  // --- Grafické metody ---
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  // AI: Audio engine a správa bank (Backend)
  std::unique_ptr<MidiPlayer> midiPlayer;
  std::unique_ptr<BankManager> bankManager;

  // AI: UI Komponenty (Frontend)
  std::unique_ptr<TrackPanelComponent> trackPanel;
  std::unique_ptr<TransportComponent> transport;
  std::unique_ptr<SF2ListComponent> sf2List;
  std::unique_ptr<MasterPanel> masterPanel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};