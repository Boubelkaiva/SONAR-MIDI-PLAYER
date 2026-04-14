/*
  ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine s FluidSynth při zachování Mute/Solo a CC logiky.
    UPDATED: Přechod z TSF na FluidSynth.
  ==============================================================================
*/

#include "MidiPlayer.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <thread>

MidiPlayer::MidiPlayer()
    : isPlaying(false), currentSampleRate(48000.0), masterVolume(1.0f),
      settings(nullptr), synth(nullptr), playheadSeconds(0.0), currentMode(MidiMode::GM)
{
    // Inicializace FluidSynth nastavení
    settings = new_fluid_settings();
    fluid_settings_setnum(settings, "synth.sample-rate", currentSampleRate);
    fluid_settings_setint(settings, "synth.polyphony", 256);

    // Aktivace efektů přímo v jádru (místo tvého externího modulu v TSF)
    fluid_settings_setstr(settings, "synth.reverb.active", "yes");
    fluid_settings_setstr(settings, "synth.chorus.active", "yes");

    synth = new_fluid_synth(settings);
    mapper = std::make_unique<MidiMapper>();

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
        channelMuted[i] = false;
        channelSolo[i] = false;
    }
    std::cout << "[MidiPlayer] Konstruktor dokončen. FluidSynth inicializován." << std::endl;
}

MidiPlayer::~MidiPlayer()
{
    if (synth)
        delete_fluid_synth(synth);
    if (settings)
        delete_fluid_settings(settings);
}

// --- LOGIKA MUTE / SOLO (Identická s tvým originálem) ---
bool MidiPlayer::isChannelAudible(int channel) const
{
    bool anySolo = false;
    for (int i = 0; i < 16; ++i)
        if (channelSolo[i])
        {
            anySolo = true;
            break;
        }

    if (anySolo)
        return channelSolo[channel];
    return !channelMuted[channel];
}

void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    fluid_settings_setnum(settings, "synth.sample-rate", sampleRate);
}

void MidiPlayer::releaseResources() {}

void MidiPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (!isPlaying || synth == nullptr || currentSampleRate <= 0)
        return;

    int samplesToRender = bufferToFill.numSamples;
    double blockDuration = (double)samplesToRender / currentSampleRate;
    double nextPlayheadTime = playheadSeconds + blockDuration;

    // --- MIDI EVENTY (Tvá původní smyčka) ---
    for (int i = 0; i < midiSequence.getNumEvents(); ++i)
    {
        auto *event = midiSequence.getEventPointer(i);
        double eventTime = event->message.getTimeStamp();

        if (eventTime >= playheadSeconds && eventTime < nextPlayheadTime)
        {
            auto &msg = event->message;
            int chan = msg.getChannel() - 1;
            if (chan < 0 || chan > 15)
                continue;

            // Filtrování NoteOn podle tvého Mute/Solo
            if (msg.isNoteOn() && !isChannelAudible(chan))
                continue;

            if (msg.isController())
            {
                int cc = msg.getControllerNumber();
                int val = msg.getControllerValue();
                if (cc == 0)
                    currentBankMSB[chan] = val;
                else if (cc == 32)
                    currentBankLSB[chan] = val;

                fluid_synth_cc(synth, chan, cc, val);
            }
            else if (msg.isProgramChange())
            {
                int prog = msg.getProgramChangeNumber();
                auto info = mapper->getInstrumentInfo(currentBankMSB[chan], prog, chan, currentMode);
                fluid_synth_bank_select(synth, chan, info.bank);
                fluid_synth_program_change(synth, chan, info.program);
            }
            else if (msg.isNoteOn())
                fluid_synth_noteon(synth, chan, msg.getNoteNumber(), msg.getVelocity());
            else if (msg.isNoteOff())
                fluid_synth_noteoff(synth, chan, msg.getNoteNumber());
            else if (msg.isPitchWheel())
                fluid_synth_pitch_bend(synth, chan, msg.getPitchWheelValue());
        }
    }

    // --- AUDIO RENDERING (FluidSynth renderuje přímo stereo) ---
    float *left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *right = (bufferToFill.buffer->getNumChannels() > 1)
                       ? bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample)
                       : nullptr;

    // FluidSynth renderuje přímo do výstupních bufferů JUCE
    fluid_synth_write_float(synth, samplesToRender, left, 0, 1, (right ? right : left), 0, 1);

    // Aplikace Master Volume na výsledek
    bufferToFill.buffer->applyGain(bufferToFill.startSample, samplesToRender, masterVolume);

    playheadSeconds = nextPlayheadTime;
    if (playheadSeconds >= midiSequence.getEndTime())
        stop();
}

void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    if (sf2File.existsAsFile())
    {
        fluid_synth_sfload(synth, sf2File.getFullPathName().toRawUTF8(), 1);
    }
}

void MidiPlayer::loadMidiFile(const juce::File &midiFile)
{
    stop();
    // Tvá původní logika s odděleným vláknem a analýzou
    std::thread([this, midiFile]() mutable
                {
        juce::MidiFile mf;
        auto is = std::unique_ptr<juce::InputStream>(midiFile.createInputStream());
        if (is == nullptr || !mf.readFrom(*is)) return;
        mf.convertTimestampTicksToSeconds();

        juce::MidiMessageSequence tempSequence;
        for (int i = 0; i < mf.getNumTracks(); ++i)
            if (auto *track = mf.getTrack(i))
                tempSequence.addSequence(*track, 0, 0, mf.getLastTimestamp());

        tempSequence.updateMatchedPairs();

        juce::MessageManager::callAsync([this, tempSeq = std::move(tempSequence)]() mutable
        {
            this->midiSequence = std::move(tempSeq);
        }); })
        .detach();
}

void MidiPlayer::play()
{
    if (synth)
        isPlaying = true;
}
void MidiPlayer::stop()
{
    isPlaying = false;
    playheadSeconds = 0.0;
}
void MidiPlayer::pause() { isPlaying = false; }
void MidiPlayer::setMasterVolume(float v) { masterVolume = v / 127.0f; }

void MidiPlayer::setChannelMute(int trackIdx, bool mute)
{
    if (trackIdx >= 0 && trackIdx < 16)
    {
        channelMuted[trackIdx] = mute;
        if (mute && synth)
            fluid_synth_all_notes_off(synth, trackIdx);
    }
}

void MidiPlayer::setChannelSolo(int trackIdx, bool solo)
{
    if (trackIdx >= 0 && trackIdx < 16)
    {
        channelSolo[trackIdx] = solo;
        if (synth)
        {
            for (int i = 0; i < 16; ++i)
                if (!isChannelAudible(i))
                    fluid_synth_all_notes_off(synth, i);
        }
    }
}

void MidiPlayer::sendRealTimeControlChange(int trackNum, int controller, int value)
{
    if (synth != nullptr)
    {
        int chan = trackNum - 1;
        if (chan >= 0 && chan < 16)
        {
            fluid_synth_cc(synth, chan, controller, value);

            // Logování přesně ve tvém stylu
            if (controller == 91 || controller == 93)
                std::cout << "[FLUID] Effect Update -> Track: " << trackNum << " | CC: " << controller << " | Val: " << value << std::endl;
        }
    }
}