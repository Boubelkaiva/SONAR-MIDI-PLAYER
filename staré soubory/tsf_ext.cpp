/*
  ==============================================================================
    FILE: tsf_ext.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: External FX Module for TSF (Reverb & Chorus support).
    AUTHOR: Iva (AI Assisted)
    DATE: 2026-04-12
  ==============================================================================
*/

#include "tsf.h"

extern "C"
{
    // Propojení CC 91 s reverbem
    void tsf_channel_set_reverb(struct tsf *f, int channel, float level)
    {
        if (f != nullptr)
            tsf_channel_midi_control(f, channel, 91, (int)(level * 127.0f));
    }

    // Propojení CC 93 s chorusem
    void tsf_channel_set_chorus(struct tsf *f, int channel, float level)
    {
        if (f != nullptr)
            tsf_channel_midi_control(f, channel, 93, (int)(level * 127.0f));
    }
}