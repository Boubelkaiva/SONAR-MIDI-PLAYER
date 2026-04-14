/*
  ==============================================================================
    FILE: MidiMapper.cpp
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)
  ==============================================================================
*/

#include "MidiMapper.h"
#include <iostream>
#include <iomanip>

MidiMapper::MidiMapper() {}
MidiMapper::~MidiMapper() {}

// Implementace chybějící funkce, kterou volá Analyzer
int MidiMapper::findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode)
{
    // Použijeme tvou logiku z getInstrumentInfo
    InstrumentInfo info = getInstrumentInfo(bankMSB, prog, channel, mode);

    // Pro FluidSynth vracíme unikátní kombinaci jako index,
    // nebo prostě program, pokud index nepotřebuješ pro pole.
    return info.program;
}

InstrumentInfo MidiMapper::getInstrumentInfo(int bankMSB, int prog, int channel, MidiMode mode)
{
    InstrumentInfo info;

    // Tvůj standard: kanál 10 (index 9) jsou vždy bicí (banka 128)
    int targetBank = (channel == 9) ? 128 : bankMSB;
    int targetProg = prog;

    // XG/GS Hybridní logika
    if (mode == MidiMode::XG && (bankMSB == 126 || bankMSB == 127))
        targetBank = 128;

    info.bank = targetBank;
    info.program = targetProg;
    info.name = "Bank: " + juce::String(targetBank) + " Prog: " + juce::String(targetProg);

    return info;
}

MidiMode MidiMapper::detectModeFromSysEx(const uint8_t *data, int size)
{
    if (!data || size < 6)
        return MidiMode::GM;

    for (int i = 0; i < size - 4; ++i)
    {
        if (data[i] == 0x43)
            return MidiMode::XG; // Yamaha
        if (data[i] == 0x41)
            return MidiMode::GS; // Roland
    }
    return MidiMode::GM;
}