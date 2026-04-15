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

  // AI: Destruktor přidaný pro jistotu kvůli správnému uvolnění unique_ptr
  ~TrackPanelComponent() override = default;

  /** Standardní JUCE metoda pro vykreslení pozadí panelu */
  void paint(juce::Graphics &g) override;

  /** Standardní JUCE metoda pro rozvržení 16 řádků (tracků) */
  void resized() override;

  // --- CALLBACKY PRO MAIN COMPONENT (PŘEMOSTĚNÍ SIGNÁLU) ---
  std::function<void(int track, int value)> onTrackVolumeChanged;
  std::function<void(int track, int value)> onTrackPanChanged;
  std::function<void(int track, int value)> onTrackReverbChanged;
  std::function<void(int track, int value)> onTrackChorusChanged;
  std::function<void(int track, bool muted)> onTrackMuteChanged;
  std::function<void(int track, bool soloed)> onTrackSoloChanged;

  /** * AI: Metoda pro hromadnou aktualizaci z analyzeru.
   * Tato metoda v .cpp volá tracks[index]->updateVolume(volume).
   */
  void updateTrackFromMetadata(int index, const juce::String &name, int volume);

  /** * AI: Synchronizace FX dat (Pan, Reverb, Chorus) z analyzeru do UI.
   */
  void setTrackFxData(int index, int pan, int reverb, int chorus);

  /** Nastavení instrumentu pro konkrétní track (0-15) */
  void setTrackInstrument(int index, const juce::String &name, juce::Colour colour);

  /** AI: Metoda pro předání MIDI aktivity (velocity) do VU metru konkrétního tracku */
  void triggerTrackVu(int index, int velocity);

private:
  // Původní nastavení na 16 tracků (standard MIDI)
  static constexpr int numTracks = 16;

  /** * Pole s tracky.
   * AI: Používáme std::array a std::unique_ptr pro bezpečnou správu paměti.
   */
  std::array<std::unique_ptr<TrackComponent>, numTracks> tracks;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanelComponent)
};