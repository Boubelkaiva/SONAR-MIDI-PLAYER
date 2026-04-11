/*
  ==============================================================================
    FILE: AudioSettingsPanel.h
    PROJECT: SONAR MIDI PLAYER
    PATH: backend/AudioSettingsPanel/AudioSettingsPanel.h
    DESCRIPTION: Komponenta pro nastavení zvukového výstupu a vzorkovací frekvence.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AudioSettingsPanel : public juce::Component
{
public:
    AudioSettingsPanel(juce::AudioDeviceManager &dm) : deviceManager(dm)
    {
        // 1. Inicializace samotného JUCE selectoru
        selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
            deviceManager,
            0, 0,  // min/max vstupy (0, protože jen přehráváme)
            2, 2,  // min/max výstupy (stereo)
            false, // schovat MIDI vstupy
            false, // schovat MIDI výstupy
            true,  // ukázat výběr Sample Rate (důležité pro tvých 48kHz)
            true   // ukázat výběr typu zařízení (WASAPI/DirectSound)
        );

        // 2. Stylování vnitřních popisků (aby byly bílé/šedé jako v Sonaru)
        // Protože selector nemá vlastní ColourId pro pozadí,
        // musíme barvy vnutit jeho vnitřním prvkům přes LookAndFeel.
        selector->setLookAndFeel(&customLookAndFeel);

        addAndMakeVisible(selector.get());
    }

    ~AudioSettingsPanel() override
    {
        selector->setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics &g) override
    {
        // Pozadí panelu (tmavě šedá s mírnou průhledností)
        g.fillAll(juce::Colour(0xff2b2b2b));

        // Decentní ohraničení
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.drawRect(getLocalBounds(), 1);
    }

    void resized() override
    {
        // Roztáhneme vnitřní selector na celou plochu s malým okrajem
        selector->setBounds(getLocalBounds().reduced(5));
    }

private:
    juce::AudioDeviceManager &deviceManager;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> selector;

    // Lokální úprava vzhledu, aby text v panelu nebyl černý na tmavém
    class SettingsLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        SettingsLookAndFeel()
        {
            setColour(juce::Label::textColourId, juce::Colours::lightgrey);
            setColour(juce::ComboBox::textColourId, juce::Colours::white);
        }
    };

    SettingsLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsPanel)
};