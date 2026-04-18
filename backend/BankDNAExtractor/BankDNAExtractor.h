/*
 ==============================================================================
    COMPONENT: BankDNAExtractor.h
    MODULE: backend/BankDNAExtractor/BankDNAExtractor.h
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)
    DESCRIPTION: Samostatná utilita pro precizní extrakci DNA (bank a presetů)
                 ze SoundFontu pomocí FluidSynth API.
    AUTHOR: Iva
 ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/**
 * Statická utilita pro analýzu SoundFont bank.
 * Tato třída extrahuje reálná čísla bank a programů ze SF2 souboru a ukládá je
 * do JSON mapy, kterou následně využívá UI pro zobrazení seznamů nástrojů.
 */
class BankDNAExtractor
{
public:
  /**
   * Analyzuje SF2/SF3 soubor a vytáhne DNA (seznam nástrojů) do JSONu.
   * FluidSynth verze prochází reálné presety a banky definované v souboru.
   * @param sf2File Soubor se SoundFontem k analýze.
   * @return true pokud byl JSON úspěšně vytvořen.
   */
  static bool extractToJSON(const juce::File &sf2File);

private:
  // Utility class – zakázání instance (podle tvého standardu).
  BankDNAExtractor() = delete;
  ~BankDNAExtractor() = delete;
  BankDNAExtractor(const BankDNAExtractor &) = delete;
  BankDNAExtractor &operator=(const BankDNAExtractor &) = delete;
};

// ======================================================
// HASH UTIL (FileHash64)
// ======================================================
int64_t computeFileHash64(const juce::File &file);