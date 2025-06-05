/*
  ==============================================================================

    UserInterfaceLayout.h
    Created: 1 Jun 2025 6:05:44pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "Utility.h"

#include "LookAndFeel.h"
#include "DialGraphics.h"
#include "MiscGraphics.h"

class StepInterface : public juce::Component, juce::Timer
{
public:
    StepInterface(MotherlyAudioProcessor& p, int index);
    ~StepInterface();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setIsStepIndex(int step);
    void setAmplitude(float amplitude);
    void timerCallback() override;
    

private:
    void setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, UserInterfaceGraphics& lookAndFeel);

    juce::Label freqLabel, toneLabel, modLabel, probLabel;
    UserInterfaceGraphics freqLookAndFeel { 3 }, toneLookAndFeel { 0 }, modLookAndFeel { 2 }, probLookAndFeel { 4 };
    juce::Slider freqSlider, toneSlider, modSlider, probSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment, toneAttachment, modAttachment, probAttachment;

    int stepIndex;
    float amplitude = 0.0f;
    bool isStepIndex = false, stepHovered = false, stepParentHovered = false;
    juce::Colour stepColour;
    
    MotherlyAudioProcessor& audioProcessor;
};

class DrumMainInterface : public juce::Component
{
public:
    DrumMainInterface(MotherlyAudioProcessor& p) : audioProcessor(p)
    {
        auto bounds = getLocalBounds().toFloat();
        float width = bounds.getWidth() * 1.2f;
        
        setStepParams(tensionLabel, tensionSlider, juce::Slider::TextBoxBelow, "Tension", tensionGraphics);
        tensionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "tension", tensionSlider);

        setStepParams(inharmLabel, inharmSlider, juce::Slider::TextBoxBelow, "Inharmoncity", inharmGraphics);
        inharmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "inharmonicity", inharmSlider);

        setStepParams(positionLabel, positionSlider, juce::Slider::TextBoxBelow, "Position", positionGraphics);
        positionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "position", positionSlider);

        setStepParams(algorithmLabel, algorithmSlider, juce::Slider::NoTextBox, "Algorithm", algorithmGraphics);
        algorithmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "algorithm", algorithmSlider);
        
        setStepParams(op1Label, op1Slider, juce::Slider::NoTextBox, "Op 1", smallDialGraphics);
        op1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op1Level", op1Slider);
        
        setStepParams(op2Label, op2Slider, juce::Slider::NoTextBox, "Op 2", smallDialGraphics);
        op2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op2Level", op2Slider);

        
        setStepParams(op3Label, op3Slider, juce::Slider::NoTextBox, "Op 3", smallDialGraphics);
        op3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op3Level", op3Slider);

        
        setStepParams(noiseLabel, noiseSlider, juce::Slider::NoTextBox, "Noise", smallDialGraphics);
        noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noiseLevel", noiseSlider);

        
        setStepParams(algorithmLabel, testSlider, juce::Slider::NoTextBox, "Algorithm", testGraphics);
        
        setStepParams(algorithmLabel, testSlider2, juce::Slider::TextBoxBelow, "Algorithm", testGraphics2);
        rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "seqRate", testSlider2);
    }
    
    ~DrumMainInterface()
    {
        tensionSlider.setLookAndFeel(nullptr);
        inharmSlider.setLookAndFeel(nullptr);
        positionSlider.setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bg = getLocalBounds().toFloat();
        bg.reduce(5, 5);
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bg, 5);
        
        juce::Colour mainColour = Colours::InterfaceMain::backgroundFill;
        
        g.setColour(mainColour);
        g.fillPath(bgFill);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float height = bounds.getHeight();

        tensionLabel.setBounds(x + 10, y + height * 0.1f, height * 0.7f, height * 0.1f);
        tensionSlider.setBounds(x + 10, y + height * 0.175f, height * 0.7f, height * 0.775f);
        
        inharmLabel.setBounds(x + 120, y + height * 0.1f, height * 0.7f, height * 0.1f);
        inharmSlider.setBounds(x + 120, y + height * 0.175f, height * 0.7f, height * 0.775f);
        
        positionLabel.setBounds(x + 220, y + height * 0.15f, height * 0.5f, height * 0.1f);
        positionSlider.setBounds(x + 220, y + height * 0.25f, height * 0.5f, height * 0.6f);

        algorithmLabel.setBounds(x + 500, y + height * 0.1f, height * 0.6f, height * 0.1f);
        algorithmSlider.setBounds(x + 500, y + height * 0.2, height * 0.7f, height * 0.7f);
        
        op1Label.setBounds(x + 300, y + height * 0.1f, height * 0.3f, height * 0.1f);
        op1Slider.setBounds(x + 300, y + height * 0.2, height * 0.3f, height * 0.3f);
        
        op2Label.setBounds(x + 300, y + height * 0.55f, height * 0.3f, height * 0.1f);
        op2Slider.setBounds(x + 300, y + height * 0.65, height * 0.3f, height * 0.3f);

        op3Label.setBounds(x + 350, y + height * 0.1f, height * 0.3f, height * 0.1f);
        op3Slider.setBounds(x + 350, y + height * 0.2, height * 0.3f, height * 0.3f);
        
        noiseLabel.setBounds(x + 350, y + height * 0.55f, height * 0.3f, height * 0.1f);
        noiseSlider.setBounds(x + 350, y + height * 0.65, height * 0.3f, height * 0.3f);
        
        testSlider2.setBounds(x + 650, y + height * 0.175, height * 0.7f, height * 0.775f);
    }
    
    
    void setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, UserInterfaceGraphics& lookAndFeel)
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
    
    
private:
    UserInterfaceGraphics tensionGraphics { 6 }, inharmGraphics { 7 }, positionGraphics { 8 }, algorithmGraphics { 5 }, testGraphics { 9 }, testGraphics2 { 10 }, smallDialGraphics { 11 };
    
    juce::Slider tensionSlider, inharmSlider, positionSlider, algorithmSlider, testSlider, op1Slider, op2Slider, op3Slider, noiseSlider, testSlider2;
    
    juce::Label tensionLabel, inharmLabel, positionLabel, algorithmLabel, op1Label, op2Label, op3Label, noiseLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment, inharmAttachment, positionAttachment, op1Attachment, op2Attachment, op3Attachment, noiseAttachment, algorithmAttachment, testAttachment, rateAttachment;

    MotherlyAudioProcessor& audioProcessor;
};

class PresetInterface : public juce::Component, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetInterface(MotherlyAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), audioProcessor(p)
    {
        addAndMakeVisible(saveButton);
        saveButton.addListener(this);
        saveButton.setLookAndFeel(&saveLAF);

        addAndMakeVisible(nextButton);
        nextButton.addListener(this);
        nextButton.setLookAndFeel(&nextLAF);

        addAndMakeVisible(prevButton);
        prevButton.addListener(this);
        prevButton.setLookAndFeel(&prevLAF);
        
        addAndMakeVisible(presetComboBox);
        presetComboBox.addListener(this);
        presetComboBox.setLookAndFeel(&comboBoxLAF);
    }
    
    ~PresetInterface()
    {
        saveButton.removeListener(this);
        nextButton.removeListener(this);
        prevButton.removeListener(this);
        presetComboBox.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        saveButton.setBounds(bounds.getX(), bounds.getY(), 40, 40);
        nextButton.setBounds(bounds.getX() + 75, bounds.getY(), 40, 40);
        prevButton.setBounds(bounds.getX() + 125, bounds.getY(), 40, 40);
        presetComboBox.setBounds(bounds.getX() + 175, bounds.getY(), 300, 40);
    }
    
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == &presetComboBox)
            presetManager.loadPreset(presetComboBox.getItemText(presetComboBox.getSelectedItemIndex()));
    }
    
    void buttonClicked(juce::Button* buttonClicked) override
    {
        if (buttonClicked == &saveButton){
            fileChooser = std::make_unique<juce::FileChooser>(
                "Enter Preset Name",
                presetManager.defaultDirectory,
                "*." + presetManager.extension);
            
            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
            {
                const auto resultFile = chooser.getResult();
                presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                loadPresetList();
            });

        } else if (buttonClicked == &nextButton){
            presetManager.loadNextPreset();
        } else if (buttonClicked == &prevButton){
            presetManager.loadPreviousPreset();
        }
    }
    
    void loadPresetList()
    {
        presetComboBox.clear(juce::dontSendNotification);
        const auto allPresets = presetManager.getAllPreset();
        const auto currentPreset = presetManager.getCurrentPreset();
        presetComboBox.addItemList(allPresets, 1);
        presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
    }
private:
    ButtonGraphics saveLAF { 0 }, nextLAF { 1 }, prevLAF { 2 };
    ComboBoxGraphics comboBoxLAF;
    
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox;
    
    std::unique_ptr<juce::FileChooser> fileChooser;

    PresetManager presetManager;
    MotherlyAudioProcessor& audioProcessor;
};

