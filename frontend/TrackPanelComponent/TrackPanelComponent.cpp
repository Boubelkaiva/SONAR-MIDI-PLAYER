/*
  ==============================================================================

    FILE: TrackPanelComponent.cpp
    NAME: TrackPanelComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation of the main track list panel.
                 FIXED: Full implementation with updateTrackFromMetadata.
                 ADDED: Real-time callback bridging for all 16 tracks.

  ==============================================================================
*/

#include "TrackPanelComponent.h"

TrackPanelComponent::TrackPanelComponent()
{
    // AI: Generujeme 16 tracků podle tvé původní logiky
    for (int i = 0; i < numTracks; ++i)
    {
        // AI: Vytvoření instance tracku s výchozím nastavením
        tracks[i] = std::make_unique<TrackComponent>(
            i + 1,
            "Track " + juce::String(i + 1),
            InstrumentType::Piano);

        // AI: Nastavení tvých ikon pro Mute, Solo a Volume tlačítka
        tracks[i]->setIcons("M", "S", "SF");

        // --- PROPOJENÍ CALLBACKŮ Z JEDNOTLIVÝCH TRACKŮ DO PANELU ---

        tracks[i]->onVolumeChanged = [this](int trk, int val)
        {
            if (onTrackVolumeChanged)
                onTrackVolumeChanged(trk - 1, val);
        };

        tracks[i]->onPanChanged = [this](int trk, int val)
        {
            if (onTrackPanChanged)
                onTrackPanChanged(trk - 1, val);
        };

        tracks[i]->onReverbChanged = [this](int trk, int val)
        {
            if (onTrackReverbChanged)
                onTrackReverbChanged(trk - 1, val);
        };

        tracks[i]->onChorusChanged = [this](int trk, int val)
        {
            if (onTrackChorusChanged)
                onTrackChorusChanged(trk - 1, val);
        };

        tracks[i]->onMuteChanged = [this](int trk, bool muted)
        {
            if (onTrackMuteChanged)
                onTrackMuteChanged(trk - 1, muted);
        };

        tracks[i]->onSoloChanged = [this](int trk, bool soloed)
        {
            if (onTrackSoloChanged)
                onTrackSoloChanged(trk - 1, soloed);
        };

        // 🔥 NOVÉ: Instrument callback 🔥
        tracks[i]->onInstrumentSelected = [this](int trk, int bank, int cat, int prog)
        {
            if (onTrackInstrumentChanged)
                onTrackInstrumentChanged(trk - 1, bank, cat, prog);
        };

        addAndMakeVisible(*tracks[i]);
    }
}

/** * AI: Klíčová metoda pro propojení Backend Analyzeru s Frontend UI.
 * MainComponent volá tuhle metodu pro každý kanál po načtení MIDI.
 */
void TrackPanelComponent::updateTrackFromMetadata(int index, const juce::String &name, int volume)
{
    if (index >= 0 && index < numTracks)
    {
        tracks[index]->setInstrument(name, juce::Colour(0xff444444));
        tracks[index]->updateVolume(volume);
    }
}

void TrackPanelComponent::setTrackFxData(int index, int pan, int reverb, int chorus)
{
    if (index >= 0 && index < numTracks)
    {
        if (tracks[index] != nullptr)
        {
            tracks[index]->updateFxData(pan, reverb, chorus);
        }
    }
}

void TrackPanelComponent::setTrackInstrument(int index, const juce::String &name, juce::Colour colour)
{
    if (index >= 0 && index < numTracks)
    {
        tracks[index]->setInstrument(name, colour);
    }
}

void TrackPanelComponent::triggerTrackVu(int index, int velocity)
{
    if (index >= 0 && index < numTracks)
    {
        if (tracks[index] != nullptr)
        {
            tracks[index]->triggerVuMeter(velocity);
        }
    }
}

void TrackPanelComponent::resized()
{
    auto r = getLocalBounds();

    if (numTracks > 0)
    {
        int trackHeight = r.getHeight() / numTracks;

        for (int i = 0; i < numTracks; ++i)
        {
            tracks[i]->setBounds(r.removeFromTop(trackHeight).reduced(0, 2));
        }
    }
}

void TrackPanelComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::transparentBlack);
}