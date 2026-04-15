/*
  ==============================================================================
    FILE: MidiAnalyzer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Logic for extracting metadata from MIDI/KAR files.
    UPDATED: Integrated MidiMapper for TSF Index verification (v1.3.0).
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Dopředná deklarace, abychom nemuseli vkládat celý header mapperu
class MidiMapper;

/** Struktura pro přenos informací o jednom MIDI kanálu do UI */
struct TrackData
{
  int channel;                 // 1-16
  int programNumber;           // 0-127 (GM Program)
  int bankMSB;                 // CC 0 - Klíčové pro Yamaha 127 / Roland 128
  int bankLSB;                 // CC 32 - Variace
  int tsfIndex;                // SKUTEČNÝ index v SoundFontu (přidáno pro debug)
  juce::String instrumentName; // Textový název nástroje
  float initialVolume;         // Hlasitost 0-127 (podle CC 7)
  juce::String trackName;      // Název tracku z Meta Eventů

  // PŘIDÁNO PRO FX MODAL:
  int initialReverb = 0; // CC 91 - Výchozí úroveň dozvuku
  int initialChorus = 0; // CC 93 - Výchozí úroveň chorusu
};

class MidiAnalyzer
{
public:
  MidiAnalyzer() = default;
  ~MidiAnalyzer() = default;

  /** * Otevře soubor a vrátí seznam dat pro všech 16 kanálů.
   * @param file MIDI soubor k analýze
   * @param mapper Ukazatel na instanci mapperu pro ověření TSF indexů
   */
  std::vector<TrackData> analyzeFile(const juce::File &file, MidiMapper *mapper);

private:
  /** * Pomocná metoda pro převod čísla programu na název.
   */
  juce::String getGMName(int p, int bank, bool isDrumChannel);
};