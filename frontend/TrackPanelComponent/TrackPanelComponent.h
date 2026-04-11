/*
  ==============================================================================

    FILE: TrackPanelComponent.h
    NAME: TrackPanelComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Header for the main track list panel (16 tracks).
                 RESTORED ORIGINAL STRUCTURE.
                 ADDED: Metadata update method for MIDI integration.
                 FIXED: Correct method signatures for TrackComponent access.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../TrackComponent/TrackComponent.h"

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

  /** * AI: Metoda pro hromadnou aktualizaci z analyzeru.
   * Tato metoda v .cpp volá tracks[index]->setTrackVolume(volume).
   */
  void updateTrackFromMetadata(int index, const juce::String &name, float volume);

  /** Nastavení instrumentu pro konkrétní track (0-15) */
  void setTrackInstrument(int index, const juce::String &name, juce::Colour colour);

private:
  // Původní nastavení na 16 tracků (standard MIDI)
  static constexpr int numTracks = 16;

  /** * Pole s tracky.
   * AI: Používáme std::array a std::unique_ptr pro bezpečnou správu paměti.
   * TrackComponent musí mít v public sekci metodu setTrackVolume(float).
   */
  std::array<std::unique_ptr<TrackComponent>, numTracks> tracks;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanelComponent)
};