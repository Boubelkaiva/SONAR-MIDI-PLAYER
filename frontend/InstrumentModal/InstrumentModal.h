/*
  ==============================================================================
    FILE: InstrumentModal.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Třísloupcový výběr nástroje (Bank, Category, Instrument).
    UPDATED: Added JSON loading support + AppData path handling
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <functional>

class InstrumentModal : public juce::Component
{
public:
    InstrumentModal(int trackIdx, std::function<void(int, int, int)> onSelected)
        : trackIndex(trackIdx), onInstrumentSelected(onSelected)
    {
        auto setupLabel = [this](juce::Label &l, const juce::String &text)
        {
            addAndMakeVisible(l);
            l.setText(text, juce::dontSendNotification);
            l.setFont(juce::Font(14.0f, juce::Font::bold));
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, juce::Colour(0xfffca503));
        };

        setupLabel(bankLabel, "BANK");
        setupLabel(categoryLabel, "CATEGORY");
        setupLabel(instrumentLabel, "INSTRUMENT");

        auto setupList = [this](juce::ListBox &lb)
        {
            addAndMakeVisible(lb);
            lb.setOutlineThickness(1);
            lb.setColour(juce::ListBox::outlineColourId, juce::Colours::white.withAlpha(0.1f));
            lb.setColour(juce::ListBox::backgroundColourId, juce::Colours::black.withAlpha(0.2f));
        };

        setupList(bankListBox);
        setupList(categoryListBox);
        setupList(instrumentListBox);

        setSize(600, 400);

        loadBankData(); // 🔥 AUTO LOAD JSON
    }

    ~InstrumentModal() override = default;

    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();

        g.setColour(juce::Colour(0xff2b2b2b));
        g.fillRoundedRectangle(bounds, 8.0f);

        g.setColour(juce::Colour(0xfffca503).withAlpha(0.4f));
        g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(15);
        area.removeFromTop(10);

        auto labelArea = area.removeFromTop(25);
        auto listArea = area;

        int columnW = listArea.getWidth() / 3;
        int spacing = 5;

        bankLabel.setBounds(labelArea.removeFromLeft(columnW));
        categoryLabel.setBounds(labelArea.removeFromLeft(columnW));
        instrumentLabel.setBounds(labelArea);

        bankListBox.setBounds(listArea.removeFromLeft(columnW).reduced(spacing));
        categoryListBox.setBounds(listArea.removeFromLeft(columnW).reduced(spacing));
        instrumentListBox.setBounds(listArea.reduced(spacing));
    }

    // ======================================================
    // DATA LOADING (AppData / bank_settings JSON)
    // ======================================================
    void loadBankData()
    {
        auto appData = juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory);

        auto jsonDir = appData
                           .getChildFile("SonarMidiPlayer")
                           .getChildFile("bank_settings");

        if (!jsonDir.exists())
            return;

        auto files = jsonDir.findChildFiles(
            juce::File::findFiles, false, "*.json");

        if (files.isEmpty())
            return;

        currentJsonFile = files[0];

        juce::FileInputStream stream(currentJsonFile);
        if (!stream.openedOk())
            return;

        juce::String jsonText = stream.readEntireStreamAsString();

        auto json = juce::JSON::parse(jsonText);

        if (!json.isObject())
            return;

        auto *obj = json.getDynamicObject();
        auto instrumentsArray = obj->getProperty("instruments");

        if (auto *arr = instrumentsArray.getArray())
            instruments = *arr;
    }

private:
    int trackIndex;
    std::function<void(int, int, int)> onInstrumentSelected;

    // UI
    juce::Label bankLabel, categoryLabel, instrumentLabel;
    juce::ListBox bankListBox, categoryListBox, instrumentListBox;

    // DATA
    juce::Array<juce::var> instruments;
    juce::File currentJsonFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentModal)
};