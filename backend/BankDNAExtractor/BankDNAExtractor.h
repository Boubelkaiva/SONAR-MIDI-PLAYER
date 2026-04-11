/*
  ==============================================================================
    COMPONENT: BankDNAExtractor.h
    MODULE: backend/BankDNAExtractor/BankDNAExtractor.h
    DESCRIPTION: Header file for the DNA Extraction component.
    AUTHOR: Iva
  ==============================================================================
*/

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class BankDNAExtractor
{
public:
    /**
     * Bleskově analyzuje SF2 soubor a vytáhne DNA (seznam nástrojů) do JSONu.
     * @param sf2File Soubor se SoundFontem (klidně i ten 1,5 GB Svatý grál).
     * @return true pokud se extrakce podařila.
     */
    static bool extractToJSON(const juce::File &sf2File);

private:
    BankDNAExtractor() = default;
};