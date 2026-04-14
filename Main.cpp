/*
  ==============================================================================
    FILE: Main.cpp
    PROJECT: SONAR MIDI PLAYER
    MODULE: Main.cpp
    DESCRIPTION: Root entry point for the AI-assisted MIDI application.
                 Handles window lifecycle and content initialization.
                 INTEGRATION: BankDNAExtractor included for bank analysis.
    VERSION: 1.2.0 (Stable Memory Management)
    AUTHOR: Iva
  ==============================================================================
*/

#include <JuceHeader.h>
#include "frontend/MainComponent/MainComponent.h"
#include "backend/BankDNAExtractor/BankDNAExtractor.h"
#include "frontend/MainComponent/Icons.h" // <--- PŘIDÁNO: Inkluze s naší ikonou

// ==============================================================================
class MainAppWindow : public juce::DocumentWindow
{
public:
    MainAppWindow(juce::String name)
        : DocumentWindow(name,
                         juce::Colours::darkgrey,
                         DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);

        // --- NASTAVENÍ IKONY ---
        // Načteme naši 16x16 ikonu a nastavíme ji oknu
        setIcon(Icons::getSonarIcon());

        // Inicializace hlavní komponenty
        mainComponent = std::make_unique<MainComponent>();
        setContentOwned(mainComponent.get(), true);

        setResizable(true, true);
        setResizeLimits(800, 600, 3840, 2160);

        centreWithSize(getWidth(), getHeight());
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

private:
    std::unique_ptr<MainComponent> mainComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAppWindow)
};

// ==============================================================================
class MainApplication : public juce::JUCEApplication
{
public:
    MainApplication() {}

    const juce::String getApplicationName() override { return "Sonar MIDI Player"; }
    const juce::String getApplicationVersion() override { return "1.2.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String &commandLine) override
    {
        mainWindow = std::make_unique<MainAppWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

private:
    std::unique_ptr<MainAppWindow> mainWindow;
};

// ==============================================================================
START_JUCE_APPLICATION(MainApplication)