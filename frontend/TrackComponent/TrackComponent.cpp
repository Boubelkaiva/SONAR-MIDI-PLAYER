/*
  ==============================================================================
    FILE: TrackComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Track UI s integrovaným VUMeterem a opraveným FX oknem.
    FIXED: Obnovena funkce showFxPopup() pro otevírání FX modalu.
  ==============================================================================
*/

#include "TrackComponent.h"
#include "../FxModal/FxModal.h"
#include "../VUMeter/VUMeter.h"
#include "../InstrumentModal/InstrumentModal.cpp"
#include <juce_gui_basics/juce_gui_basics.h>

// --- LOOK AND FEEL ---
class ToggleButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics &g, juce::Button &button,
                              const juce::Colour &, bool, bool isMouseOverButton) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        juce::Colour bg;

        if (button.getToggleState() || button.isDown())
        {
            if (button.getButtonText() == "M")
                bg = juce::Colours::red;
            else if (button.getButtonText() == "S")
                bg = juce::Colours::yellow;
            else if (button.getButtonText() == "FX")
                bg = juce::Colours::orange;
            else
                bg = juce::Colours::green;
        }
        else
        {
            bg = juce::Colours::grey.withAlpha(0.3f);
        }

        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(isMouseOverButton ? juce::Colours::white.withAlpha(0.4f) : juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        g.setColour(button.getToggleState() ? juce::Colours::black : juce::Colours::white);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }
    void drawButtonText(juce::Graphics &, juce::TextButton &, bool, bool) override {}
};

static ToggleButtonLookAndFeel toggleButtonLF;

// =======================================================
// CONSTRUCTOR
// =======================================================
TrackComponent::TrackComponent(int trackNumber,
                               const juce::String &instrumentName,
                               InstrumentType type)
    : trackNum(trackNumber),
      trackName(instrumentName),
      instrType(type)
{
    addAndMakeVisible(trackNumberButton);
    trackNumberButton.setButtonText(juce::String(trackNum));
    trackNumberButton.setColour(juce::TextButton::textColourOffId, trackNumberTextColor);
    trackNumberButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    trackNumberButton.setEnabled(false);

    addAndMakeVisible(nameLabel);
    nameLabel.setText(trackName, juce::dontSendNotification);
    nameLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xff333333));

    vuMeter = std::make_unique<VUMeter>();
    addAndMakeVisible(vuMeter.get());

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0, 127, 1);
    volumeSlider.setValue(currentVolume);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);
    volumeSlider.onValueChange = [this]
    {
        currentVolume = (int)volumeSlider.getValue();
        std::cout << "[POG] FE: Track " << trackNum << " -> UI VOLUME slider: " << currentVolume << std::endl;
        if (onVolumeChanged)
            onVolumeChanged(trackNum, currentVolume);
    };

    addAndMakeVisible(fxButton);
    fxButton.setButtonText("FX");
    fxButton.setLookAndFeel(&toggleButtonLF);
    fxButton.onClick = [this]
    { showFxPopup(); };

    addAndMakeVisible(muteButton);
    muteButton.setButtonText("M");
    muteButton.setClickingTogglesState(true);
    muteButton.setLookAndFeel(&toggleButtonLF);
    muteButton.onClick = [this]
    {
        isMuted = muteButton.getToggleState();
        if (onMuteChanged)
            onMuteChanged(trackNum, isMuted);
    };

    addAndMakeVisible(soloButton);
    soloButton.setButtonText("S");
    soloButton.setClickingTogglesState(true);
    soloButton.setLookAndFeel(&toggleButtonLF);
    soloButton.onClick = [this]
    {
        isSoloed = soloButton.getToggleState();
        if (onSoloChanged)
            onSoloChanged(trackNum, isSoloed);
    };

    addAndMakeVisible(thirdButton);
    thirdButton.setButtonText("SF");
    thirdButton.setLookAndFeel(&toggleButtonLF);

    // 🔥 NAPOJENÍ NA MODAL 🔥
    thirdButton.onClick = [this]
    { showInstrumentPopup(); };
}

TrackComponent::~TrackComponent()
{
    muteButton.setLookAndFeel(nullptr);
    soloButton.setLookAndFeel(nullptr);
    thirdButton.setLookAndFeel(nullptr);
    fxButton.setLookAndFeel(nullptr);
}

void TrackComponent::resized()
{
    auto r = getLocalBounds().reduced(2);

    trackNumberButton.setBounds(r.removeFromLeft(45));
    nameLabel.setBounds(r.removeFromLeft(135));

    r.removeFromLeft(4);
    if (vuMeter != nullptr)
        vuMeter->setBounds(r.removeFromLeft(8).reduced(0, 4));

    r.removeFromLeft(4);

    int w = 35;
    thirdButton.setBounds(r.removeFromRight(w));
    soloButton.setBounds(r.removeFromRight(w));
    muteButton.setBounds(r.removeFromRight(w));
    fxButton.setBounds(r.removeFromRight(w));

    volumeSlider.setBounds(r.reduced(4, 2));
}

void TrackComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff222222));
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
}

void TrackComponent::triggerVuMeter(int velocity)
{
    if (vuMeter != nullptr)
        vuMeter->trigger(velocity);
}

void TrackComponent::updateVolume(int newVolume)
{
    currentVolume = newVolume;
    volumeSlider.setValue(newVolume, juce::dontSendNotification);
}

void TrackComponent::updateMuteState(bool isMutedState)
{
    isMuted = isMutedState;
    muteButton.setToggleState(isMuted, juce::dontSendNotification);
}

void TrackComponent::updateSoloState(bool isSoloedState)
{
    isSoloed = isSoloedState;
    soloButton.setToggleState(isSoloed, juce::dontSendNotification);
}

void TrackComponent::updateFxData(int pan, int reverb, int chorus)
{
    currentPan = pan;
    currentReverb = reverb;
    currentChorus = chorus;
}

// FX modal (BEZE ZMĚN)
void TrackComponent::showFxPopup()
{
    FxModal::Listener fxListener;

    fxListener.onPanChanged = [this](int val)
    {
        currentPan = val;
        if (onPanChanged)
            onPanChanged(trackNum, val);
    };

    fxListener.onReverbChanged = [this](int val)
    {
        currentReverb = val;
        if (onReverbChanged)
            onReverbChanged(trackNum, val);
    };

    fxListener.onChorusChanged = [this](int val)
    {
        currentChorus = val;
        if (onChorusChanged)
            onChorusChanged(trackNum, val);
    };

    auto *modal = new FxModal(trackNum, fxListener);
    modal->setInitialValues(currentPan, currentReverb, currentChorus);

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(modal);
    options.resizable = false;
    options.content->setSize(360, 220);
    options.dialogTitle = "Track " + juce::String(trackNum) + " FX Settings";
    options.componentToCentreAround = this;
    options.dialogBackgroundColour = juce::Colour(0xff2b2b2b);
    options.useNativeTitleBar = true;
    options.launchAsync();
}

// 🔥 NOVÁ FUNKCE 🔥
void TrackComponent::showInstrumentPopup()
{
    auto onSelected = [this](int bank, int cat, int prog)
    {
        std::cout << "Selected: B:" << bank << " C:" << cat << " P:" << prog << std::endl;

        // 🔥 PŘEDÁNÍ NA PANEL / BE 🔥
        if (onInstrumentSelected)
            onInstrumentSelected(trackNum, bank, cat, prog);
    };

    auto *modal = new InstrumentModal(trackNum - 1, onSelected);

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(modal);
    options.resizable = false;
    options.content->setSize(600, 400);
    options.dialogTitle = "Track " + juce::String(trackNum) + " Instrument";
    options.componentToCentreAround = this;
    options.dialogBackgroundColour = juce::Colour(0xff2b2b2b);
    options.useNativeTitleBar = true;
    options.launchAsync();
}

void TrackComponent::setInstrument(const juce::String &name, juce::Colour colour)
{
    nameLabel.setText(name, juce::dontSendNotification);
}

void TrackComponent::setIcons(const juce::String &mute, const juce::String &solo, const juce::String &third)
{
    muteButton.setButtonText(mute);
    soloButton.setButtonText(solo);
    thirdButton.setButtonText(third);
}