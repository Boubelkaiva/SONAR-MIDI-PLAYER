/*
  ==============================================================================

    FILE: TrackPanelComponent.cpp
    NAME: TrackPanelComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation of the main track list panel.
                 FIXED: Full implementation with updateTrackFromMetadata.

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

        addAndMakeVisible(*tracks[i]);
    }
}

/** * AI: Klíčová metoda pro propojení Backend Analyzeru s Frontend UI.
 * MainComponent volá tuhle metodu pro každý kanál po načtení MIDI.
 */
void TrackPanelComponent::updateTrackFromMetadata(int index, const juce::String &name, float volume)
{
    if (index >= 0 && index < numTracks)
    {
        // Změň juce::Colours::cyan na něco jiného, například tmavě šedou nebo oranžovou
        // tracks[index]->setInstrument(name, juce::Colours::darkgrey); // decentní šedá
        tracks[index]->setInstrument(name, juce::Colour(0xff444444)); // vlastní barva

        tracks[index]->updateVolume(volume);
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