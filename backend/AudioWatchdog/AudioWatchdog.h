/*
  ==============================================================================

    AudioWatchdog.h
    Created: 14 Apr 2026
    Author: AI System (Iva's Project)
    Description: Samostatná komponenta pro automatickou obnovu audio spojení
                 při odpojení hardwaru (sluchátek) na 32-bit systémech.
                 Sleduje změny v AudioDeviceManageru a vynucuje restart portů.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * AudioWatchdog sleduje stav zvukového rozhraní systému.
 * Pokud detekuje stav "<< none >>", vynutí re-inicializaci výchozího zařízení (reproduktorů).
 */
class AudioWatchdog : public juce::ChangeListener
{
public:
    /** Konstruktor vyžaduje referenci na tvůj hlavní AudioDeviceManager */
    AudioWatchdog(juce::AudioDeviceManager &dm)
        : deviceManager(dm)
    {
        // Přihlásíme se k odběru zpráv o změnách v hardwaru
        deviceManager.addChangeListener(this);
    }

    /** Při zániku komponenty se korektně odhlásíme */
    ~AudioWatchdog() override
    {
        deviceManager.removeChangeListener(this);
    }

    /** * Hlavní metoda, kterou JUCE zavolá, jakmile Windows nahlásí změnu (např. vytažení Jacku).
     * Běží na Message Threadu, takže je bezpečná pro práci s UI a Device Managerem.
     */
    void changeListenerCallback(juce::ChangeBroadcaster *source) override
    {
        if (source == &deviceManager)
        {
            auto setup = deviceManager.getAudioDeviceSetup();

            // Pokud log ukazuje prázdno nebo "<< none >>", jednáme.
            if (setup.outputDeviceName.isEmpty() || setup.outputDeviceName == "<< none >>")
            {
                // Výpis do tvého logu pro kontrolu
                DBG("[WATCHDOG] Sluchátka ztracena. Aktivuji záchranný protokol...");
                std::cout << "[AI SYSTEM] Audio výstup přerušen. Restartuji zařízení..." << std::endl;

                // 1. Krok: Uzavření mrtvého portu (uvolnění Ghost Device)
                deviceManager.closeAudioDevice();

                // 2. Krok: Vynucené očichání dostupného HW (Windows nám teď musí dát repráky)
                // Parametry (0, 2) znamenají 0 vstupů a 2 výstupy (stereo)
                auto error = deviceManager.initialiseWithDefaultDevices(0, 2);

                if (error.isEmpty())
                {
                    auto newSetup = deviceManager.getAudioDeviceSetup();
                    std::cout << "[AI SYSTEM] Spojení obnoveno na: " << newSetup.outputDeviceName << std::endl;
                }
                else
                {
                    std::cerr << "[ERROR] Watchdog nemohl najít žádný zvukový hardware: " << error << std::endl;
                }
            }
        }
    }

private:
    juce::AudioDeviceManager &deviceManager;

    // Standardní JUCE makro pro bezpečnost paměti
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioWatchdog)
};