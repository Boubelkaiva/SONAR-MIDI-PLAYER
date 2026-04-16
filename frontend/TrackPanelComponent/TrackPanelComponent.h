/*
  ==============================================================================

    FILE: TrackPanelComponent.h
    NAME: TrackPanelComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Header for the main track list panel (16 tracks).
                 RESTORED ORIGINAL STRUCTURE.
                 ADDED: Metadata update method for MIDI integration.
                 FIXED: Correct method signatures for TrackComponent access.
                 ADDED: Real-time FX callbacks for MainComponent.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../TrackComponent/TrackComponent.h"
#include <functional>

class TrackPanelComponent : public juce::Component
{
public:
  TrackPanelComponent();

  ~TrackPanelComponent() override = default;

  void paint(juce::Graphics &g) override;
  void resized() override;

  // --- CALLBACKY PRO MAIN COMPONENT (PŘEMOSTĚNÍ SIGNÁLU) ---
  std::function<void(int track, int value)> onTrackVolumeChanged;
  std::function<void(int track, int value)> onTrackPanChanged;
  std::function<void(int track, int value)> onTrackReverbChanged;
  std::function<void(int track, int value)> onTrackChorusChanged;
  std::function<void(int track, bool muted)> onTrackMuteChanged;
  std::function<void(int track, bool soloed)> onTrackSoloChanged;

  // 🔥 NOVÉ: Instrument callback 🔥
  std::function<void(int track, int bank, int category, int program)> onTrackInstrumentChanged;

  // --- AKTUALIZACE STAVU ---

  void updateTrackFromMetadata(int index, const juce::String &name, int volume);

  void setTrackFxData(int index, int pan, int reverb, int chorus);

  void setTrackInstrument(int index, const juce::String &name, juce::Colour colour);

  void triggerTrackVu(int index, int velocity);

private:
  static constexpr int numTracks = 16;

  std::array<std::unique_ptr<TrackComponent>, numTracks> tracks;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanelComponent)
};