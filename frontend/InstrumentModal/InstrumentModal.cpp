/*
 ==============================================================================
   FILE: InstrumentModal.cpp
   DESCRIPTION: Implementation of InstrumentModal (UI + layout only)
   DEBUG: Terminal logging with module prefixes (BE inspection)
 ==============================================================================
*/

#include "InstrumentModal.h"

// ======================================================
// DEBUG MACRO STYLE LOGGING
// ======================================================
#define IM_LOG(msg) DBG("[InstrumentModal] " + juce::String(msg))
#define IM_BE(msg) DBG("[InstrumentModal][BE] " + juce::String(msg))
#define IM_UI(msg) DBG("[InstrumentModal][UI] " + juce::String(msg))
#define IM_WARN(msg) DBG("[InstrumentModal][WARN] " + juce::String(msg))
#define IM_ERR(msg) DBG("[InstrumentModal][ERROR] " + juce::String(msg))

// ======================================================
// DEBUG: BE PACKET INSPECTION
// ======================================================
static void debugBE(int bank,
                    int category,
                    int program,
                    const juce::String &name)
{
  IM_BE("========================================");
  IM_BE("BANK     : " + juce::String(bank));
  IM_BE("CATEGORY : " + juce::String(category));
  IM_BE("PROGRAM  : " + juce::String(program));
  IM_BE("NAME     : " + name);

  if (bank < 0)
    IM_ERR("Invalid bank (<0) → BankDNA broken or missing");

  if (program < 0)
    IM_ERR("Invalid program (<0) → SF2 corruption");

  if (category == 0)
    IM_WARN("Category = 0 → CategoryResolver not applied");

  if (name == "UNKNOWN")
    IM_WARN("Name not mapped → InstrumentMap missing");

  IM_BE("========================================");
}

// ======================================================
// CONSTRUCTOR
// ======================================================
InstrumentModal::InstrumentModal(int trackIdx,
                                 std::function<void(int, int, int)> onSelected)
    : trackIndex(trackIdx),
      onInstrumentSelected(onSelected)
{
  setSize(600, 400);

  IM_LOG("INIT modal for track " + juce::String(trackIdx));

  auto setupList = [this](juce::ListBox &lb)
  {
    addAndMakeVisible(lb);

    lb.setOutlineThickness(1);
    lb.setColour(juce::ListBox::outlineColourId,
                 juce::Colours::grey.withAlpha(0.5f));
    lb.setColour(juce::ListBox::backgroundColourId,
                 juce::Colours::black.withAlpha(0.2f));

    IM_UI("ListBox created");
  };

  setupList(bankListBox);
  setupList(categoryListBox);
  setupList(instrumentListBox);

  // 🔥 DEBUG BE SAMPLE (only inspection)
  debugBE(-1, 0, 999, "AcPno1");

  // 🚨 IMPORTANT STATE DEBUG
  IM_WARN("Modal created BUT NO DATA MODEL INJECTED YET → UI WILL BE EMPTY");
}

// ======================================================
// ⚠️ NEW: DATA BINDING HOOK (zatím jen stub debug)
// ======================================================
void InstrumentModal::setModel(const juce::var &model)
{
  IM_BE("MODEL RECEIVED FROM BANKMANAGER");

  // 🔥 TEMP DEBUG - tady uvidíš co jde z BE
  IM_BE("RAW MODEL: " + model.toString());

  IM_WARN("Model not applied to ListBoxes yet (binding missing)");

  // TODO NEXT STEP:
  // - parse banks
  // - fill bankListBox
  // - cascade category/instrument
}

// ======================================================
// DESTRUCTOR
// ======================================================
InstrumentModal::~InstrumentModal()
{
  IM_LOG("DESTROYED");
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

  IM_UI("paint() called");
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

  IM_UI("Layout updated");
}