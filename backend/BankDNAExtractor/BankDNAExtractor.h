/*
  ==============================================================================
    COMPONENT: BankDNAExtractor.h
    MODULE: backend/BankDNAExtractor/BankDNAExtractor.h
    DESCRIPTION: Samostatná utilita pro bleskovou extrakci DNA ze SoundFontu.
                 Provádí se při nahrávání do paměti, aby player zůstal odlehčený.
    AUTHOR: Iva
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/**
 * Statická utilita pro analýzu SoundFont bank.
 * Tato třída je navržena tak, aby vytvořila JSON mapu nástrojů ještě předtím,
 * než se banka plně inicializuje v MIDI přehrávači.
 */
class BankDNAExtractor
{
public:
  /**
   * Analyzuje SF2/SF3 soubor a vytáhne DNA (seznam nástrojů) do JSONu.
   * Volá se v momentě loadování do paměti.
   * * @param sf2File Soubor se SoundFontem k analýze.
   * @return true pokud byl JSON úspěšně vytvořen.
   */
  static bool extractToJSON(const juce::File &sf2File);

private:
  // AI: Utility class nesmí mít instanci.
  BankDNAExtractor() = delete;
  ~BankDNAExtractor() = delete;
};