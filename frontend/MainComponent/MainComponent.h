/*
  ==============================================================================
    FILE: MainComponent.h
    PROJECT: SONAR MIDI PLAYER
    MODULE: frontend/MainComponent
    DESCRIPTION: Main container - handling audio and UI layout.
    UPDATED: Full integration of Real-time FX and Audio HW switching.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Frontend komponenty
#include "../TrackPanelComponent/TrackPanelComponent.h"
#include "../TransportComponent/TransportComponent.h"
#include "../SF2ListComponent/SF2ListComponent.h"
#include "../MasterPanel/MasterPanel.h"

// Backend moduly
#include "../../backend/MidiPlayer/MidiPlayer.h"
#include "../../backend/BankManager/BankManager.h"
#include "../../backend/MidiAnalyzer/MidiAnalyzer.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::ChangeListener // Sleduje změny HW (sluchátka/repro)
{
public:
  MainComponent();
  ~MainComponent() override;

  // --- AUDIO METODY ---
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  // --- GRAFICKÉ METODY ---
  void paint(juce::Graphics &g) override;
  void resized() override;

  // --- CALLBACKY ---
  void changeListenerCallback(juce::ChangeBroadcaster *source) override;

  /** AI FIX: Vynutí inicializaci audia v momentě, kdy je komponenta vložena do okna.
      Řeší problém s tichem po změně inicializace v Main.cpp. */
  void parentHierarchyChanged() override;

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