/**
 * @file main.cpp
 * @brief Test přehrání tónu pomocí FluidSynth s více bankami
 * @description
 * Jednoduchá ukázka použití knihovny FluidSynth.
 * Program načte více SoundFont (.sf2/.sf3), zahraje jeden tón (C4) na 2 sekundy
 * a poté jej vypne.
 *
 * ⚠️ POZOR:
 * Uprav cesty k .sf2/.sf3 souborům podle svého systému.
 *
 * @author Ivana
 * @date 2026
 */

#include <iostream>
#include <vector>
#include <string>
#include <fluidsynth.h>
#include <thread>
#include <chrono>
#include <windows.h> // pro nastavení UTF-8

void drawWindow()
{
    std::cout << "===============================" << std::endl;
    std::cout << "|       SONAR MIDI PLAYER     |" << std::endl;
    std::cout << "|                             |" << std::endl;
    std::cout << "|  Přehraje se C4 tón na 2s   |" << std::endl;
    std::cout << "|                             |" << std::endl;
    std::cout << "===============================" << std::endl;
}

int main()
{
    // Nastavení konzole na UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    drawWindow();

    fluid_settings_t *settings = new_fluid_settings();
    fluid_synth_t *synth = new_fluid_synth(settings);
    fluid_audio_driver_t *adriver = new_fluid_audio_driver(settings, synth);

    std::vector<std::string> sf_paths = {
        "E:\\CAKEWALK\\SONAR 2024\\Roland_GM.sf2",
        "E:\\CAKEWALK\\SONAR 2024\\Yamaha_XG.sf2"};

    for (const auto &path : sf_paths)
    {
        int sfid = fluid_synth_sfload(synth, path.c_str(), 1);
        if (sfid == -1)
            std::cout << "Nepodařilo se načíst SF: " << path << std::endl;
        else
            std::cout << "SF načten úspěšně: " << path << std::endl;
    }

    // zahraj tón
    fluid_synth_noteon(synth, 0, 60, 100);
    std::cout << "Přehrávání C4..." << std::endl;

    // jednoduchý progress bar
    for (int i = 0; i < 20; i++)
    {
        std::cout << "#";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << std::endl;

    fluid_synth_noteoff(synth, 0, 60);

    delete_fluid_audio_driver(adriver);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);

    std::cout << "Hotovo! Stiskni Enter pro ukončení..." << std::endl;
    std::cin.get();

    return 0;
}