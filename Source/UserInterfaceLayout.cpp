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
    probLookAndFeel.setStepIndex(stepIndex);

    // initialize sliders and labels
    setStepParams(freqLabel, freqSlider, juce::Slider::TextBoxBelow, "Freq", freqLookAndFeel);
    juce::String freqID = "freq" + juce::String(stepIndex);
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, freqID, freqSlider);

    setStepParams(toneLabel, toneSlider, juce::Slider::TextBoxBelow, "Tone", toneLookAndFeel);
    juce::String toneID = "tone" + juce::String(stepIndex);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, toneID, toneSlider);

    setStepParams(modLabel, modSlider, juce::Slider::NoTextBox, "Mod", modLookAndFeel);
    juce::String modID = "mod" + juce::String(stepIndex);
    modAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, modID, modSlider);

    setStepParams(probLabel, probSlider, juce::Slider::TextBoxBelow, "Prob", probLookAndFeel);
    juce::String probID = "prob" + juce::String(stepIndex);
    probAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, probID, probSlider);

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
    auto bg = getLocalBounds().toFloat();
    bg.reduce(5, 5);
        
    juce::Colour mainColour = Colours::InterfaceMain::backgroundFill;
    juce::Colour hoverColour = Colours::InterfaceMain::backgroundHoverFill;
    juce::Colour bgFillColour = stepHovered ? hoverColour : mainColour;

    juce::Path bgFill;
    bgFill.addRoundedRectangle(bg, 5);

    // figure out some logic here for hover/on vs neutral
    juce::Colour blinkerFillColour { Colours::StepColour::stepMainColour[stepIndex].withAlpha(0.15f) };
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

    freqLabel.setBounds(x, y + height * 0.025f, width, width/4);
    freqSlider.setBounds(x, y + height * 0.025f, width, width);

    toneLabel.setBounds(x, y + height * 0.3, width, width/4);
    toneSlider.setBounds(x, y + height * 0.3, width, width);

    modLabel.setBounds(x, y + height * 0.575f, width, width/4);
    modSlider.setBounds(x, y + height * 0.575f, width, width * 0.75f);

    probLabel.setBounds(x, y + height * 0.725f, width, width/4);
    probSlider.setBounds(x, y + height * 0.725f, width, width);
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

void StepInterface::setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, UserInterfaceGraphics& lookAndFeel)
{
    // initialize label
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, Colours::InterfaceMain::textColor);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(12.0f);

    // slider
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
    slider.setLookAndFeel(&lookAndFeel);
}
