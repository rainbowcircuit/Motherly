#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "Utility.h"

#include "LookAndFeel.h"
#include "DialGraphics.h"
#include "MiscGraphics.h"
#include "PatchBayGraphics.h"

class PresetInterface : public juce::Component, juce::Timer, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetInterface(MotherlyAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts);
    ~PresetInterface();
    
    void changeRateValueSlider();
    void paint(juce::Graphics& g) override {}
    
    void resized() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonClicked) override;
    void loadPresetList();
private:
    void timerCallback() override;
    
    UserInterfaceGraphics rateLAF { 9 };
    ButtonGraphics saveLAF { 0 }, nextLAF { 1 }, prevLAF { 2 };
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
    DrumMainInterface(MotherlyAudioProcessor& p);
    ~DrumMainInterface();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setSliderAndLabel(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel);
    
    
private:
    UserInterfaceGraphics tensionLAF { 6 }, inharmLAF { 7 }, positionLAF { 8 }, algorithmLAF { 5 }, outputLAF { 10 }, smallDialLAF { 11 }, noiseFreqLAF { 1 };
    
    ButtonGraphics stateLAF { 0 };
    
    juce::Slider tensionSlider, inharmSlider, positionSlider, algorithmSlider, testSlider, op1Slider, op2Slider, op3Slider, noiseSlider, testSlider2, noiseFreqSlider, outputSlider;
    
    juce::Label tensionLabel, inharmLabel, positionLabel, algorithmLabel, op1Label, op2Label, op3Label, noiseLabel, noiseFreqLabel, outputLabel;
    
    juce::ToggleButton stateButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stateAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment, inharmAttachment, positionAttachment, op1Attachment, op2Attachment, op3Attachment, noiseAttachment, noiseFreqAttachment, algorithmAttachment, testAttachment, outputAttachment;

    MotherlyAudioProcessor& audioProcessor;
};

