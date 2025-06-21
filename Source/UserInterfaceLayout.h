#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "Utility.h"

#include "ColorLookAndFeel.h"
#include "DialLookAndFeel.h"
#include "MiscLookAndFeel.h"
#include "PatchBayGraphics.h"

class InterfaceHelper
{
protected:
    void setSliderAndLabel(juce::Component& parent, juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel)
    {
        // initialize label
        parent.addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, Colors::Main::textColor);
        label.setJustificationType(juce::Justification::centred);
        juce::FontOptions font { 12.0f, juce::Font::plain };
        label.setFont(font);

        // slider
        parent.addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(textBoxPosition, false, 68, 20);
        slider.setTextValueSuffix(suffix);
        slider.setLookAndFeel(&lookAndFeel);
    }
};

class PresetInterface : public juce::Component, juce::Timer, juce::ComboBox::Listener, juce::Button::Listener, InterfaceHelper
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
    
    UserInterfaceGraphics rateLAF { 11 };
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

class StepInterface : public juce::Component, juce::Timer, InterfaceHelper
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
    juce::Label pitchLabel,
    toneLabel,
    modLabel,
    repeatLabel;
    
    UserInterfaceGraphics pitchLAF { 0 },
    toneLAF { 1 },
    modLAF { 2 },
    repeatLAF { 3 };
    
    juce::Slider pitchSlider,
    toneSlider,
    modSlider,
    repeatSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment,
    toneAttachment,
    modAttachment,
    repeatAttachment;

    int stepIndex;
    float amplitude = 0.0f;
    bool isStepIndex = false, stepHovered = false, stepParentHovered = false;
    juce::Colour stepColour;
    
    MotherlyAudioProcessor& audioProcessor;
};

class DrumMainInterface : public juce::Component, InterfaceHelper
{
public:
    DrumMainInterface(MotherlyAudioProcessor& p);
    ~DrumMainInterface();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void outputSliderDisplay();
    
private:
    UserInterfaceGraphics tensionLAF { 4 },
    inharmLAF { 5 },
    positionLAF { 6 },
    smallDialLAF { 7 },
    noiseFreqLAF { 8 },
    algorithmLAF { 9 },
    outputLAF { 10 };
    
    ButtonGraphics stateLAF { 0 };
    
    juce::Slider tensionSlider,
    inharmSlider,
    positionSlider,
    algorithmSlider,
    op1Slider,
    op2Slider,
    op3Slider,
    noiseSlider,
    noiseFreqSlider,
    outputSlider;
    
    juce::Label tensionLabel,
    inharmLabel,
    positionLabel,
    algorithmLabel,
    op1Label,
    op2Label,
    op3Label,
    noiseLabel,
    noiseFreqLabel,
    outputLabel;
    
    juce::ToggleButton stateButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stateAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tensionAttachment, inharmAttachment, positionAttachment, op1Attachment, op2Attachment, op3Attachment, noiseAttachment, noiseFreqAttachment, algorithmAttachment, testAttachment, outputAttachment;

    MotherlyAudioProcessor& audioProcessor;
};

