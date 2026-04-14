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
        tracks[i]->setIcons("M", "S", "Vol");

        // --- PROPOJENÍ CALLBACKŮ Z JEDNOTLIVÝCH TRACKŮ DO PANELU ---
        // Používáme i (index 0-15) pro identifikaci tracku v panelu
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

        // Propojení Reverb slideru
        tracks[i]->onReverbChanged = [this](int trk, int val)
        {
            if (onTrackReverbChanged)
                onTrackReverbChanged(trk - 1, val);
        };

        // Propojení Chorus slideru
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
        // Nastavení barvy a jména instrumentu
        tracks[index]->setInstrument(name, juce::Colour(0xff444444)); // vlastní tmavě šedá barva

        tracks[index]->updateVolume(volume);
    }
}

/** * AI: Synchronizace FX dat (Pan, Reverb, Chorus) z analyzeru do konkrétního tracku.
 * Metoda, která chyběla a kterou volá MainComponent.
 */
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

/** Ruční nastavení instrumentu pro konkrétní track (např. při změně v UI) */
void TrackPanelComponent::setTrackInstrument(int index, const juce::String &name, juce::Colour colour)
{
    if (index >= 0 && index < numTracks)
    {
        tracks[index]->setInstrument(name, colour);
    }
}

void TrackPanelComponent::resized()
{
    auto r = getLocalBounds();

    // AI: Rozdělení celkové výšky panelu mezi 16 tracků
    if (numTracks > 0)
    {
        int trackHeight = r.getHeight() / numTracks;

        for (int i = 0; i < numTracks; ++i)
        {
            // AI: .reduced(0, 2) vytvoří malou mezeru mezi tracky pro lepší přehlednost
            tracks[i]->setBounds(r.removeFromTop(trackHeight).reduced(0, 2));
        }
    }
}

void TrackPanelComponent::paint(juce::Graphics &g)
{
    // AI: Pozadí panelu (pokud není překryto tracky)
    g.fillAll(juce::Colours::transparentBlack);
}