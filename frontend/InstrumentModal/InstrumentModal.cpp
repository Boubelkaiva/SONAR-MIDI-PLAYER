/*
  ==============================================================================
    FILE: InstrumentModal.cpp
    DESCRIPTION: Implementation of InstrumentModal (UI + layout only)
  ==============================================================================
*/

#include "InstrumentModal.h"
#include <juce_gui_basics/juce_gui_basics.h>

// ======================================================
// CONSTRUCTOR
// ======================================================
InstrumentModal::InstrumentModal(int trackIdx,
                                 std::function<void(int, int, int)> onSelected)
    : trackIndex(trackIdx),
      onInstrumentSelected(onSelected)
{
    setSize(600, 400);

    auto setupList = [this](juce::ListBox &lb)
    {
        addAndMakeVisible(lb);
        lb.setOutlineThickness(1);
        lb.setColour(juce::ListBox::outlineColourId,
                     juce::Colours::grey.withAlpha(0.5f));
        lb.setColour(juce::ListBox::backgroundColourId,
                     juce::Colours::black.withAlpha(0.2f));
    };

    setupList(bankListBox);
    setupList(categoryListBox);
    setupList(instrumentListBox);
}

// ======================================================
// PAINT
// ======================================================
void InstrumentModal::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff2b2b2b));

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);

    g.drawText("SELECT INSTRUMENT FOR TRACK " + juce::String(trackIndex + 1),
               getLocalBounds().removeFromTop(30),
               juce::Justification::centred);
}

// ======================================================
// RESIZED
// ======================================================
void InstrumentModal::resized()
{
    auto area = getLocalBounds().reduced(10);
    area.removeFromTop(30);

    int columnW = area.getWidth() / 3;

    bankListBox.setBounds(area.removeFromLeft(columnW).reduced(2));
    categoryListBox.setBounds(area.removeFromLeft(columnW).reduced(2));
    instrumentListBox.setBounds(area.reduced(2));
}