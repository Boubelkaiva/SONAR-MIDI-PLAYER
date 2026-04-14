/*
  ==============================================================================
    FILE: MidiMapper.h
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cstdint>

enum class MidiMode
{
    GM,
    GS,
    XG
};

// Struktura pro přenos výsledku mapování
struct InstrumentInfo
{
    int bank;
    int program;
    juce::String name;
};

class MidiMapper
{
public:
    MidiMapper(); // Konstruktor bez TSF
    ~MidiMapper();

    // Tato funkce musí existovat, protože ji volá MidiAnalyzer
    int findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode);

    // Tuto funkci používáš v CPP pro získání detailů
    InstrumentInfo getInstrumentInfo(int bankMSB, int prog, int channel, MidiMode mode);

    static MidiMode detectModeFromSysEx(const uint8_t *data, int size);

private:
    // AI: FluidSynth nepotřebuje držet instanci fontu zde, řeší si to synth v MidiPlayeru
};