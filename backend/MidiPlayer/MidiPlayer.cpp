/*
 ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: FIXED AUDIO ENGINE (WORKING FLUIDSYNTH PIPELINE)
 ==============================================================================
*/

#include "MidiPlayer.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"
#include <iostream>
#include <thread>
#include <cmath>

#define LOG(x) std::cout << x << std::endl

// =========================
// CONSTRUCTOR
// =========================
MidiPlayer::MidiPlayer()
    : isPlaying(false),
      currentSampleRate(48000.0),
      masterVolume(1.0f),
      settings(nullptr),
      synth(nullptr),
      playheadSeconds(0.0),
      currentMode(MidiMode::GM)
{
    LOG("[MidiPlayer] CONSTRUCTOR START");

    settings = new_fluid_settings();

    fluid_settings_setnum(settings, "synth.sample-rate", currentSampleRate);
    fluid_settings_setint(settings, "synth.polyphony", 256);
    fluid_settings_setnum(settings, "synth.gain", 1.0);

    synth = new_fluid_synth(settings);

    // 🔥 FIX: FORCE clean GM state immediately
    fluid_synth_system_reset(synth);

    mapper = std::make_unique<MidiMapper>();

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
        channelMuted[i] = false;
        channelSolo[i] = false;

        // 🔥 FIX: ensure valid program on all channels
        fluid_synth_program_change(synth, i, 0);
    }

    LOG("[MidiPlayer] INIT OK");
}

// =========================
// DESTRUCTOR
// =========================
MidiPlayer::~MidiPlayer()
{
    LOG("[MidiPlayer] DESTRUCTOR");

    if (synth)
        delete_fluid_synth(synth);

    if (settings)
        delete_fluid_settings(settings);
}

// =========================
// AUDIO INIT
// =========================
void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    LOG("[AUDIO] prepareToPlay sampleRate=" << sampleRate);

    if (sampleRate <= 0)
        return;

    bool reinit = (std::abs(currentSampleRate - sampleRate) > 0.01);

    if (!reinit)
        return;

    LOG("[AUDIO] REINIT SYNTH");

    if (synth)
    {
        delete_fluid_synth(synth);
        synth = nullptr;
    }

    currentSampleRate = sampleRate;

    fluid_settings_setnum(settings, "synth.sample-rate", currentSampleRate);

    synth = new_fluid_synth(settings);

    fluid_synth_system_reset(synth);

    fluid_synth_set_gain(synth, 1.0f);

    // 🔥 FIX: restore GM program state after rebuild
    for (int i = 0; i < 16; ++i)
        fluid_synth_program_change(synth, i, 0);

    if (!lastSf2Path.isEmpty())
    {
        LOG("[SF2] reload");
        fluid_synth_sfload(synth, lastSf2Path.toRawUTF8(), 1);
    }

    LOG("[AUDIO] SYNTH READY");
}

// =========================
// AUDIO CALLBACK
// =========================
void MidiPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (!isPlaying || synth == nullptr)
        return;

    const int samples = bufferToFill.numSamples;

    float *left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *right = (bufferToFill.buffer->getNumChannels() > 1)
                       ? bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample)
                       : nullptr;

    float *outL = left;
    float *outR = right ? right : left;

    fluid_synth_write_float(
        synth,
        samples,
        outL, 0, 1,
        outR, 0, 1);

    bufferToFill.buffer->applyGain(bufferToFill.startSample, samples, masterVolume);
}

// =========================
// SOUND FONT
// =========================
void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    if (!sf2File.existsAsFile())
        return;

    lastSf2Path = sf2File.getFullPathName();

    LOG("[SF2] load");

    if (synth)
    {
        fluid_synth_sfload(synth, lastSf2Path.toRawUTF8(), 1);

        // 🔥 FIX: ensure audible state after load
        fluid_synth_system_reset(synth);

        for (int i = 0; i < 16; ++i)
            fluid_synth_program_change(synth, i, 0);
    }
}

// =========================
// PLAYBACK CONTROL
// =========================
void MidiPlayer::play()
{
    LOG("[PLAY]");

    if (synth)
    {
        fluid_synth_system_reset(synth);

        for (int i = 0; i < 16; ++i)
            fluid_synth_program_change(synth, i, 0);
    }

    isPlaying = true;
}

void MidiPlayer::stop()
{
    LOG("[STOP]");
    isPlaying = false;

    if (synth)
        fluid_synth_all_sounds_off(synth, -1);
}