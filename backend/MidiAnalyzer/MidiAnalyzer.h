/*
  ==============================================================================
    FILE: MidiAnalyzer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Logika pro extrakci metadat z MIDI/KAR souborů.
    UPDATED: Přechod na FluidSynth – nahrazení TSF Indexu (v2.0.0).
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Dopředná deklarace pro minimalizaci závislostí
class MidiMapper;

/** * Struktura pro přenos informací o jednom MIDI kanálu do UI.
 * Uchovává stav kanálu zjištěný hloubkovou analýzou MIDI eventů.
 */
struct TrackData
{
    int channel;       // 1-16
    int programNumber; // 0-127 (MIDI Program Change)
    int bankMSB;       // CC 0 (Klíčové pro Yamaha XG / Roland GS)
    int bankLSB;       // CC 32 (Variace / Bank Select LSB)

    // Změněno z tsfIndex na presetIndex pro kompatibilitu s FluidSynth
    int presetIndex; // Skutečné ID nástroje v aktuálním SoundFontu

    juce::String instrumentName; // Textový název nástroje (GM nebo SF2 název)
    float initialVolume;         // Hlasitost 0-127 (z CC 7)
    juce::String trackName;      // Název tracku z Meta Eventů (pokud existuje)

    // Data pro tvůj FX MODAL:
    int initialReverb = 0; // CC 91 - Výchozí úroveň Reverb sendu
    int initialChorus = 0; // CC 93 - Výchozí úroveň Chorus sendu

    // Zpětná kompatibilita (volitelné):
    // Pokud máš v UI hodně odkazů na název tsfIndex, můžeš nechat alias:
    int &tsfIndex = presetIndex;
};

class MidiAnalyzer
{
public:
    MidiAnalyzer() = default;
    ~MidiAnalyzer() = default;

    /** * Otevře soubor a vrátí seznam dat pro všech 16 kanálů.
     * @param file MIDI soubor k analýze
     * @param mapper Ukazatel na instanci mapperu pro ověření FluidSynth bank
     */
    std::vector<TrackData> analyzeFile(const juce::File &file, MidiMapper *mapper);

private:
    /** * Pomocná metoda pro převod čísla programu a banky na srozumitelný název.
     */
    juce::String getGMName(int p, int bank, bool isDrumChannel);
};