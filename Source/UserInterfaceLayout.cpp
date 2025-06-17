/*
  ==============================================================================

    UserInterfaceLayout.cpp
    Created: 1 Jun 2025 6:05:44pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "UserInterfaceLayout.h"
#include "Utility.h"
StepInterface::StepInterface(MotherlyAudioProcessor& p, int index) : audioProcessor(p)
{
    stepIndex = index;
    freqLookAndFeel.setStepIndex(stepIndex);
    toneLookAndFeel.setStepIndex(stepIndex);
    modLookAndFeel.setStepIndex(stepIndex);
    repeatLookAndFeel.setStepIndex(stepIndex);

    // initialize sliders and labels
    setStepParams(freqLabel, freqSlider, juce::Slider::TextBoxBelow, "Pitch", " Hz", freqLookAndFeel);
    juce::String freqID = "freq" + juce::String(stepIndex);
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, freqID, freqSlider);

    setStepParams(toneLabel, toneSlider, juce::Slider::TextBoxBelow, "Tone", " %", toneLookAndFeel);
    juce::String toneID = "tone" + juce::String(stepIndex);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, toneID, toneSlider);

    setStepParams(modLabel, modSlider, juce::Slider::NoTextBox, "P.Env", "", modLookAndFeel);
    juce::String modID = "mod" + juce::String(stepIndex);
    modAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, modID, modSlider);

    setStepParams(repeatLabel, repeatSlider, juce::Slider::TextBoxBelow, "Repeat", "", repeatLookAndFeel);
    juce::String probID = "repeat" + juce::String(stepIndex);
    repeatAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, probID, repeatSlider);

    startTimerHz(30);
}


StepInterface::~StepInterface()
{
    freqSlider.setLookAndFeel(nullptr);
    toneSlider.setLookAndFeel(nullptr);
    modSlider.setLookAndFeel(nullptr);
}

void StepInterface::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(5, 5);
        
    juce::Colour mainColour = Colours::Main::backgroundFill;
    juce::Colour hoverColour = Colours::Main::backgroundHoverFill;
    juce::Colour bgFillColour = stepHovered ? hoverColour : mainColour;

    juce::Path bgFill;
    bgFill.addRoundedRectangle(bounds, 5);

    juce::Colour blinkerFillColour { Colours::Main::backgroundActiveFill };
    juce::Colour fillColour =  isStepIndex ? blinkerFillColour : bgFillColour;

    g.setColour(fillColour);
    g.fillPath(bgFill);
}

void StepInterface::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    freqLabel.setBounds(x, y + height * 0.035f, width, width/4);
    freqSlider.setBounds(x, y + height * 0.035f, width, width);

    toneLabel.setBounds(x, y + height * 0.325, width, width/4);
    toneSlider.setBounds(x, y + height * 0.325, width, width * 1.15f);

    modLabel.setBounds(x, y + height * 0.625f, width, width/4);
    modSlider.setBounds(x, y + height * 0.625f, width, width * 0.75f);

    repeatLabel.setBounds(x, y + height * 0.785f, width, width/4);
    repeatSlider.setBounds(x, y + height * 0.785f, width, width * 0.75f);
}

void StepInterface::setIsStepIndex(int step)
{
    StepInterface::isStepIndex = stepIndex == step;
}

void StepInterface::setAmplitude(float amplitude)
{
    this->amplitude = amplitude;
}

void StepInterface::timerCallback()
{
    stepHovered = isMouseOver(true);
}

void StepInterface::setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel)
{
    // initialize label
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, Colours::Main::textColor);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(12.0f);

    // slider
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(textBoxPosition, false, 75, 20);
    slider.setTextValueSuffix(suffix);
    slider.setLookAndFeel(&lookAndFeel);
}
