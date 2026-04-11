/*
  ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Clean Audio Engine with safety guards.
    FIXED: Audio buffer overflow and nullptr safety to prevent crashes.
  ==============================================================================
*/

#define TSF_IMPLEMENTATION
#include "tsf.h"
#include "MidiPlayer.h"
#include <iostream>
#include <iomanip>

MidiPlayer::MidiPlayer()
    : isPlaying(false), currentSampleRate(44100.0), masterVolume(1.0f),
      g_tinyfont(nullptr), playheadSeconds(0.0), currentMode(MidiMode::GM)
{
    mapper = std::make_unique<MidiMapper>(nullptr);

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
    }
}

MidiPlayer::~MidiPlayer()
{
    if (g_tinyfont)
        tsf_close(g_tinyfont);
}

void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;

    // POJISTKA: TSF interleaved potřebuje 2x tolik místa v jednom poli (L+R)
    renderBuffer.setSize(1, samplesPerBlockExpected * 2);

    if (g_tinyfont)
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)sampleRate, 0.0f);
}

void MidiPlayer::releaseResources()
{
    renderBuffer.setSize(0, 0);
}

void MidiPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    // KRITICKÁ POJISTKA: Pokud nemáme font nebo nehrajeme, okamžitě ven
    if (!isPlaying || g_tinyfont == nullptr || currentSampleRate <= 0)
        return;

    double blockDuration = (double)bufferToFill.numSamples / currentSampleRate;
    double nextPlayheadTime = playheadSeconds + blockDuration;

    // 1. Zpracování MIDI zpráv pro tento blok
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

            if (msg.isController())
            {
                int cc = msg.getControllerNumber();
                int val = msg.getControllerValue();
                if (cc == 0)
                    currentBankMSB[chan] = val;
                else if (cc == 32)
                    currentBankLSB[chan] = val;
                else
                    tsf_channel_midi_control(g_tinyfont, chan, cc, val);
            }
            else if (msg.isProgramChange())
            {
                int prog = msg.getProgramChangeNumber();
                if (mapper)
                {
                    int idx = mapper->findDeepPresetIndex(currentBankMSB[chan], prog, chan, currentMode);
                    if (idx >= 0)
                        tsf_channel_set_presetindex(g_tinyfont, chan, idx);
                }
            }
            else if (msg.isNoteOn())
                tsf_channel_note_on(g_tinyfont, chan, msg.getNoteNumber(), msg.getFloatVelocity());
            else if (msg.isNoteOff())
                tsf_channel_note_off(g_tinyfont, chan, msg.getNoteNumber());
            else if (msg.isPitchWheel())
                tsf_channel_set_pitchwheel(g_tinyfont, chan, msg.getPitchWheelValue());
        }
    }

    // 2. Renderování audia
    renderBuffer.clear();
    // Musíme zajistit, že nečteme víc, než kolik má renderBuffer místa
    int samplesToRender = bufferToFill.numSamples;
    tsf_render_float(g_tinyfont, renderBuffer.getWritePointer(0), samplesToRender, 0);

    // 3. Distribuce do výstupních kanálů (L/R)
    const float *tsfOut = renderBuffer.getReadPointer(0);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        float *deviceOut = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        for (int i = 0; i < samplesToRender; ++i)
        {
            // Indexování: i*2 je levý kanál, i*2 + 1 je pravý
            deviceOut[i] = tsfOut[i * 2 + (channel % 2)] * masterVolume;
        }
    }

    playheadSeconds = nextPlayheadTime;
    if (playheadSeconds >= midiSequence.getEndTime())
        stop();
}

void MidiPlayer::loadMidiFile(const juce::File &midiFile)
{
    stop();
    juce::MidiFile mf;
    if (auto is = midiFile.createInputStream())
    {
        mf.readFrom(*is);
        mf.convertTimestampTicksToSeconds();

        currentMode = MidiMode::GM;
        for (int t = 0; t < mf.getNumTracks(); ++t)
        {
            auto *track = mf.getTrack(t);
            for (int i = 0; i < track->getNumEvents(); ++i)
            {
                auto &msg = track->getEventPointer(i)->message;
                if (msg.isSysEx())
                    currentMode = MidiMapper::detectModeFromSysEx((const uint8_t *)msg.getSysExData(), msg.getSysExDataSize());
                if (msg.getTimeStamp() > 0.5)
                    break;
            }
        }

        midiSequence.clear();
        for (int i = 0; i < mf.getNumTracks(); ++i)
            midiSequence.addSequence(*mf.getTrack(i), 0, 0, mf.getLastTimestamp());

        midiSequence.updateMatchedPairs();
    }
}

void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    // Před načtením raději stopneme audio
    isPlaying = false;

    if (g_tinyfont)
        tsf_close(g_tinyfont);

    g_tinyfont = tsf_load_filename(sf2File.getFullPathName().toRawUTF8());

    if (g_tinyfont)
    {
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)currentSampleRate, 0.0f);
        if (mapper)
            mapper->updateTSFInstance(g_tinyfont);

        std::cout << "[MidiPlayer] Font naložen: " << sf2File.getFileName() << std::endl;
    }
}

void MidiPlayer::play()
{
    if (g_tinyfont)
        isPlaying = true;
}

void MidiPlayer::stop()
{
    isPlaying = false;
    playheadSeconds = 0.0;
    if (g_tinyfont)
        tsf_reset(g_tinyfont);
}

void MidiPlayer::pause() { isPlaying = false; }

void MidiPlayer::setMasterVolume(float v)
{
    masterVolume = v / 127.0f;
}