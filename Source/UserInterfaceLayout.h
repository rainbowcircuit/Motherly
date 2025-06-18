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
#include "PatchBayGraphics.h"

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
    void setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel);

    juce::Label freqLabel, toneLabel, modLabel, repeatLabel;
    UserInterfaceGraphics freqLookAndFeel { 3 }, toneLookAndFeel { 0 }, modLookAndFeel { 2 }, repeatLookAndFeel { 4 };
    juce::Slider freqSlider, toneSlider, modSlider, repeatSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment, toneAttachment, modAttachment, repeatAttachment;

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
        setStepParams(tensionLabel, tensionSlider, juce::Slider::TextBoxBelow, "Tension", " %", tensionGraphics);
        tensionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "tension", tensionSlider);

        setStepParams(inharmLabel, inharmSlider, juce::Slider::TextBoxBelow, "Inharmoncity", " %", inharmGraphics);
        inharmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "inharmonicity", inharmSlider);

        setStepParams(positionLabel, positionSlider, juce::Slider::TextBoxBelow, "Position", " %", positionGraphics);
        positionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "position", positionSlider);

        setStepParams(algorithmLabel, algorithmSlider, juce::Slider::NoTextBox, "Algorithm", "", algorithmGraphics);
        algorithmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "algorithm", algorithmSlider);
        
        setStepParams(op1Label, op1Slider, juce::Slider::NoTextBox, "Op 1", "", smallDialGraphics);
        op1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op1Level", op1Slider);
        
        setStepParams(op2Label, op2Slider, juce::Slider::NoTextBox, "Op 2", "", smallDialGraphics);
        op2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op2Level", op2Slider);

        
        setStepParams(op3Label, op3Slider, juce::Slider::NoTextBox, "Op 3", "", smallDialGraphics);
        op3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op3Level", op3Slider);

        
        setStepParams(noiseLabel, noiseSlider, juce::Slider::NoTextBox, "Noise", "", smallDialGraphics);
        noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noiseLevel", noiseSlider);

        setStepParams(noiseFreqLabel, noiseFreqSlider, juce::Slider::TextBoxBelow, "Freq", " % ", noiseFreqGraphics);
        noiseFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noiseFreq", noiseFreqSlider);

        //setStepParams(algorithmLabel, testSlider, juce::Slider::NoTextBox, "Algorithm", testGraphics);
        
        setStepParams(outputLabel, outputSlider, juce::Slider::TextBoxBelow, "Output", " dB", testGraphics2);
        outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "outputGain", outputSlider);
        
        addAndMakeVisible(stateButton);
        stateButton.setToggleable(true);
        stateButton.setClickingTogglesState(true);
        stateButton.setLookAndFeel(&stateLAF);
        stateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "active", stateButton);

    }
    
    ~DrumMainInterface()
    {
        tensionSlider.setLookAndFeel(nullptr);
        inharmSlider.setLookAndFeel(nullptr);
        positionSlider.setLookAndFeel(nullptr);
        op1Slider.setLookAndFeel(nullptr);
        op2Slider.setLookAndFeel(nullptr);
        op3Slider.setLookAndFeel(nullptr);
        noiseSlider.setLookAndFeel(nullptr);
        noiseFreqSlider.setLookAndFeel(nullptr);
        algorithmSlider.setLookAndFeel(nullptr);
        outputSlider.setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bg = getLocalBounds().toFloat();
        bg.reduce(5, 5);
        
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bg, 5);
        
        g.setColour(Colours::Main::backgroundFill);
        g.fillPath(bgFill);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        tensionLabel.setBounds(x + width * 0.01f, y + height * 0.1f, height * 0.7f, height * 0.1f);
        tensionSlider.setBounds(x + width * 0.01f, y + height * 0.175f, height * 0.7f, height * 0.775f);
        
        inharmLabel.setBounds(x + width * 0.1625f, y + height * 0.1f, height * 0.7f, height * 0.1f);
        inharmSlider.setBounds(x + width * 0.1625f, y + height * 0.175f, height * 0.7f, height * 0.775f);
        
        positionLabel.setBounds(x + width * 0.315f, y + height * 0.175f, height * 0.5f, height * 0.1f);
        positionSlider.setBounds(x + width * 0.315f, y + height * 0.25f, height * 0.5f, height * 0.6f);

        op1Label.setBounds(x + width * 0.4425f, y + height * 0.1f, height * 0.3f, height * 0.1f);
        op1Slider.setBounds(x + width * 0.4425f, y + height * 0.2, height * 0.3f, height * 0.3f);
        
        op2Label.setBounds(x + width * 0.4425f, y + height * 0.525f, height * 0.3f, height * 0.1f);
        op2Slider.setBounds(x + width * 0.4425f, y + height * 0.625, height * 0.3f, height * 0.3f);

        op3Label.setBounds(x + width * 0.5125f, y + height * 0.1f, height * 0.3f, height * 0.1f);
        op3Slider.setBounds(x + width * 0.5125f, y + height * 0.2, height * 0.3f, height * 0.3f);
        
        noiseLabel.setBounds(x + width * 0.5125f, y + height * 0.525f, height * 0.3f, height * 0.1f);
        noiseSlider.setBounds(x + width * 0.5125f, y + height * 0.625, height * 0.3f, height * 0.3f);
        
        noiseFreqLabel.setBounds(x + width * 0.582f, y + height * 0.15f, height * 0.5f, height * 0.1f);
        noiseFreqSlider.setBounds(x + width * 0.582f, y + height * 0.25f, height * 0.5f, height * 0.6f);

        algorithmLabel.setBounds(x + width * 0.6625f, y + height * 0.1f, height * 0.6f, height * 0.1f);
        algorithmSlider.setBounds(x + width * 0.6625f, y + height * 0.2, height * 0.7f, height * 0.7f);

        outputLabel.setBounds(x + width * 0.80f, y + height * 0.1f, height * 0.7f, height * 0.1f);
        outputSlider.setBounds(x + width * 0.80f, y + height * 0.175, height * 0.7f, height * 0.775f);
        
        stateButton.setBounds(x + width * 0.935f, y + height * 0.4125f, height * 0.175f, height * 0.175f);
    }
    
    
    void setStepParams(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel)
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
        slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
        slider.setTextValueSuffix(suffix);
        slider.setLookAndFeel(&lookAndFeel);
        
    }
    
    
private:
    UserInterfaceGraphics tensionGraphics { 6 }, inharmGraphics { 7 }, positionGraphics { 8 }, algorithmGraphics { 5 }, testGraphics { 9 }, testGraphics2 { 10 }, smallDialGraphics { 11 }, noiseFreqGraphics { 1 };
    
    ButtonGraphics stateLAF { 0 };
    
    juce::Slider tensionSlider, inharmSlider, positionSlider, algorithmSlider, testSlider, op1Slider, op2Slider, op3Slider, noiseSlider, testSlider2, noiseFreqSlider, outputSlider;
    
    juce::Label tensionLabel, inharmLabel, positionLabel, algorithmLabel, op1Label, op2Label, op3Label, noiseLabel, noiseFreqLabel, outputLabel;
    
    juce::ToggleButton stateButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stateAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment, inharmAttachment, positionAttachment, op1Attachment, op2Attachment, op3Attachment, noiseAttachment, noiseFreqAttachment, algorithmAttachment, testAttachment, outputAttachment;

    MotherlyAudioProcessor& audioProcessor;
};

class PresetInterface : public juce::Component, juce::Timer, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetInterface(MotherlyAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), audioProcessor(p)
    {
        addAndMakeVisible(rateSlider);
        rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        rateSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        rateSlider.setLookAndFeel(&rateGraphics);
        rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "seqRate", rateSlider);

        addAndMakeVisible(rateLabel);
        rateLabel.setText("Rate", juce::dontSendNotification);
        rateLabel.setColour(juce::Label::textColourId, Colours::Main::textColor);
        rateLabel.setJustificationType(juce::Justification::left);
        rateLabel.setFont(12.0f);
        
        addAndMakeVisible(rateValueLabel);
        rateValueLabel.setColour(juce::Label::textColourId, Colours::Main::textColor);
        rateValueLabel.setJustificationType(juce::Justification::right);
        rateValueLabel.setFont(12.0f);
        
        
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
        // refresh presets
        loadPresetList();
        patchBayInterface = std::make_unique<PatchBay>(audioProcessor);
        
        startTimerHz(20);
    }
    
    ~PresetInterface()
    {
        saveButton.removeListener(this);
        nextButton.removeListener(this);
        prevButton.removeListener(this);
        presetComboBox.removeListener(this);
    }
    
    void changeRateValueSlider()
    {
        int rate = audioProcessor.apvts.getRawParameterValue("seqRate")->load();
        std::array<juce::String, 7> rateValue = { "1/2", "3/8", "1/4", "3/16", "1/8", "3/16", "1/16" };
        rateValueLabel.setText(rateValue[rate], juce::dontSendNotification);
    }
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float height = bounds.getHeight();

        rateLabel.setBounds(600, y + height * 0.25f, 164, height * 0.35f);
        rateValueLabel.setBounds(605, y + height * 0.25f, 164, height * 0.35f);
        rateSlider.setBounds(605, y + height * 0.6f, 164, height * 0.4f - 5);

        saveButton.setBounds(x, y, height, height);
        prevButton.setBounds(x + height, y, height, height);
        nextButton.setBounds(x + height * 9.0f, y, height, height);
        presetComboBox.setBounds(x + height * 2.0f, bounds.getY(), height * 7.0f, height);
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
            loadPresetList();
            patchBayInterface->refreshFromParameters();
            
        } else if (buttonClicked == &prevButton){
            presetManager.loadPreviousPreset();
            loadPresetList();
            patchBayInterface->refreshFromParameters();
        }
        
    }
    
    void loadPresetList()
    {
        presetComboBox.clear(juce::dontSendNotification);
        const auto allPresets = presetManager.getAllPreset();
        const auto currentPreset = presetManager.getCurrentPreset();
        presetComboBox.addItemList(allPresets, 1);
        presetComboBox.setTitle(currentPreset);
        presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
    }
    
private:
    void timerCallback() override
    {
        changeRateValueSlider();
    }
    
    UserInterfaceGraphics rateGraphics { 9 };
    ButtonGraphics saveLAF { 0 }, nextLAF { 1 }, prevLAF { 3 };
    ComboBoxGraphics comboBoxLAF;
    
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox;
    juce::Slider rateSlider;
    juce::Label rateLabel, rateValueLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<PatchBay> patchBayInterface;

    PresetManager presetManager;
    MotherlyAudioProcessor& audioProcessor;
};

