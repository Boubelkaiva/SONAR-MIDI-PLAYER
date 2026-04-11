/*
  ==============================================================================
    FILE: MidiMapper.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Deep Scan Preset Mapping using original TSF structure layout.
    FIXED: Removed duplicate TSF implementation to solve LNK2005.
  ==============================================================================
*/

#include "MidiMapper.h"
#include <iostream>

// --- RUČNÍ DEFINICE STRUKTUR (Musí odpovídat tsf.h) ---
// Tímto se vyhneme inkluzi tsf.h a konfliktům s linkerem,
// ale kompilátor bude vědět, kde hledat banky a presety.

struct tsf_preset
{
    int bank, preset;
    const char *name;
};

struct tsf
{
    struct tsf_preset *presets;
    int presetNum; // Podle Iva: presetNum
    // ... ostatní pole nás nezajímají, jsou až za tímto
};

MidiMapper::MidiMapper(struct tsf *fontInstance) : g_tinyfont(fontInstance) {}
MidiMapper::~MidiMapper() {}

int MidiMapper::findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode)
{
    // Přetypujeme naši lokální definici
    struct tsf *f = (struct tsf *)g_tinyfont;

    if (!f || !f->presets)
        return -1;

    int targetBank = bankMSB;
    bool isDrumChannel = (channel == 9);

    if (isDrumChannel)
    {
        targetBank = 128;
    }
    else if (mode == MidiMode::XG && (bankMSB == 126 || bankMSB == 127))
    {
        targetBank = 128;
        isDrumChannel = true;
    }

    int fallbackIndex = -1;

    // Používáme tvé ověřené názvy: presetNum a presets
    for (int i = 0; i < f->presetNum; ++i)
    {
        int sfBank = f->presets[i].bank;
        int sfProg = f->presets[i].preset;

        if (sfBank == targetBank && sfProg == prog)
        {
            return i;
        }

        if (isDrumChannel && sfBank == 128 && fallbackIndex == -1)
        {
            fallbackIndex = i;
        }

        if (!isDrumChannel && sfBank == 0 && sfProg == prog && fallbackIndex == -1)
        {
            fallbackIndex = i;
        }
    }

    if (fallbackIndex != -1)
        return fallbackIndex;
    return (f->presetNum > 0) ? 0 : -1;
}

MidiMode MidiMapper::detectModeFromSysEx(const uint8_t *data, int size)
{
    if (!data || size < 6)
        return MidiMode::GM;
    for (int i = 0; i < size - 4; ++i)
    {
        if (data[i] == 0x43)
            return MidiMode::XG;
        if (data[i] == 0x41)
            return MidiMode::GS;
    }
    return MidiMode::GM;
}