/*
  ==============================================================================
    FILE: MidiMapper.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Logic for mapping MIDI Banks/Programs to TSF Preset Indices.
    UPDATED: Added updateTSFInstance and guard headers.
  ==============================================================================
*/

#pragma once

#include <cstdint>

// Dopředná deklarace struktury tsf z tsf.h
struct tsf;

/**
 * Podporované MIDI módy pro inteligentní mapování nástrojů.
 */
enum class MidiMode
{
    GM,
    GS,
    XG
};

class MidiMapper
{
public:
    MidiMapper(struct tsf *fontInstance);
    ~MidiMapper();

    /** * Hlavní vyhledávací logika.
     * Najde v naloženém SoundFontu nejbližší odpovídající instrument.
     */
    int findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode);

    /** * Detekuje mód (GM/GS/XG) podle SysEx zprávy na začátku MIDI souboru.
     */
    static MidiMode detectModeFromSysEx(const uint8_t *data, int size);

    /** * Aktualizuje instanci SoundFontu, se kterou mapper pracuje.
     * Nutné volat při každém loadu nového SF2 v MidiPlayeru.
     */
    void updateTSFInstance(struct tsf *newTsf)
    {
        g_tinyfont = newTsf;
    }

private:
    struct tsf *g_tinyfont;
};