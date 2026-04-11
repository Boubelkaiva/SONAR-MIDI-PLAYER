/*
  ==============================================================================
    FILE: TrackComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Track UI + FxModal integration (PAN / REVERB / CHORUS)
    FIXED: Removed redundant +64 offsets to keep MIDI 0-127 range.
  ==============================================================================
*/

#include "TrackComponent.h"
#include "../FxModal/FxModal.h"
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

        g.setColour(isMouseOverButton
                        ? juce::Colours::white.withAlpha(0.4f)
                        : juce::Colours::black.withAlpha(0.5f));

        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        g.setColour(button.getToggleState() ? juce::Colours::black : juce::Colours::white);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(),
                         juce::Justification::centred, 1);
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
    trackNumberButton.setEnabled(false);

    addAndMakeVisible(nameLabel);
    nameLabel.setText(trackName, juce::dontSendNotification);
    nameLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xff333333));

    // VOLUME SLIDER (CC 7)
    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0, 127, 1);
    volumeSlider.setValue(currentVolume);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 18);

    volumeSlider.onValueChange = [this]
    {
        currentVolume = (int)volumeSlider.getValue();
        if (onVolumeChanged)
            onVolumeChanged(trackNum, currentVolume);
    };

    // FX BUTTON
    addAndMakeVisible(fxButton);
    fxButton.setButtonText("FX");
    fxButton.setLookAndFeel(&toggleButtonLF);
    fxButton.onClick = [this]
    { showFxPopup(); };

    // MUTE BUTTON
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

    // SOLO BUTTON
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
    thirdButton.setButtonText("Vol");
    thirdButton.setLookAndFeel(&toggleButtonLF);
}

// =======================================================
// DESTRUCTOR
// =======================================================
TrackComponent::~TrackComponent()
{
    muteButton.setLookAndFeel(nullptr);
    soloButton.setLookAndFeel(nullptr);
    thirdButton.setLookAndFeel(nullptr);
    fxButton.setLookAndFeel(nullptr);
}

// =======================================================
// FX MODAL OPEN (OPRAVENO: PŘÍMÉ MIDI HODNOTY)
// =======================================================
void TrackComponent::showFxPopup()
{
    juce::Component *topParent = getParentComponent();
    while (topParent != nullptr && topParent->getParentComponent() != nullptr)
        topParent = topParent->getParentComponent();

    if (topParent == nullptr)
        return;

    struct ModalContainer
    {
        FxModal *modal = nullptr;
    };
    auto *container = new ModalContainer();

    container->modal = new FxModal(
        trackNum,
        FxModal::Listener{
            // PAN (CC 10) - Hodnota v je 0-127 z FxModalu
            [this](int v)
            {
                currentPan = v;
                if (onPanChanged)
                    onPanChanged(trackNum, currentPan);
            },

            // REVERB (CC 91) - Hodnota v je 0-127
            [this](int v)
            {
                currentReverb = v;
                if (onReverbChanged)
                    onReverbChanged(trackNum, currentReverb);
            },

            // CHORUS (CC 93) - Hodnota v je 0-127
            [this](int v)
            {
                currentChorus = v;
                if (onChorusChanged)
                    onChorusChanged(trackNum, currentChorus);
            },

            // CLOSE
            [topParent, container]()
            {
                if (container->modal != nullptr)
                {
                    topParent->removeChildComponent(container->modal);
                    delete container->modal;
                    delete container;
                }
            }});

    topParent->addAndMakeVisible(container->modal);
    container->modal->setCentrePosition(topParent->getLocalBounds().getCentreX(),
                                        topParent->getLocalBounds().getCentreY());
}

// =======================================================
// PAINT / LAYOUT / UPDATES
// =======================================================
void TrackComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff222222));
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
}

void TrackComponent::resized()
{
    auto r = getLocalBounds().reduced(2);
    trackNumberButton.setBounds(r.removeFromLeft(45));
    nameLabel.setBounds(r.removeFromLeft(135));

    int w = 35;
    thirdButton.setBounds(r.removeFromRight(w));
    soloButton.setBounds(r.removeFromRight(w));
    muteButton.setBounds(r.removeFromRight(w));
    fxButton.setBounds(r.removeFromRight(w));

    volumeSlider.setBounds(r.reduced(4, 2));
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

void TrackComponent::setInstrument(const juce::String &name, juce::Colour colour)
{
    nameLabel.setText(name, juce::dontSendNotification);
    nameLabel.setColour(juce::Label::backgroundColourId, colour);
}

void TrackComponent::setIcons(const juce::String &mute,
                              const juce::String &solo,
                              const juce::String &third)
{
    muteButton.setButtonText(mute);
    soloButton.setButtonText(solo);
    thirdButton.setButtonText(third);
}