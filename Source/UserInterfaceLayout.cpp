#include "UserInterfaceLayout.h"
#include "Utility.h"

PresetInterface::PresetInterface(MotherlyAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), audioProcessor(p)
{
    addAndMakeVisible(rateSlider);
    rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    rateSlider.setLookAndFeel(&rateLAF);
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

PresetInterface::~PresetInterface()
{
    saveButton.removeListener(this);
    nextButton.removeListener(this);
    prevButton.removeListener(this);
    presetComboBox.removeListener(this);
}

void PresetInterface::changeRateValueSlider()
{
    int rate = audioProcessor.apvts.getRawParameterValue("seqRate")->load();
    std::array<juce::String, 7> rateValue = { "1/2", "3/8", "1/4", "3/16", "1/8", "3/16", "1/16" };
    rateValueLabel.setText(rateValue[rate], juce::dontSendNotification);
}

void PresetInterface::resized()
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

void PresetInterface::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetComboBox)
        presetManager.loadPreset(presetComboBox.getItemText(presetComboBox.getSelectedItemIndex()));
}

void PresetInterface::buttonClicked(juce::Button* buttonClicked)
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

void PresetInterface::loadPresetList()
{
    presetComboBox.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPreset();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetComboBox.addItemList(allPresets, 1);
    presetComboBox.setTitle(currentPreset);
    presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}

void PresetInterface::timerCallback()
{
    changeRateValueSlider();
}

StepInterface::StepInterface(MotherlyAudioProcessor& p, int index) : audioProcessor(p)
{
    stepIndex = index;
    pitchLAF.setStepIndex(stepIndex);
    toneLAF.setStepIndex(stepIndex);
    modLAF.setStepIndex(stepIndex);
    repeatLAF.setStepIndex(stepIndex);

    // initialize sliders and labels
    setSliderAndLabel(*this, pitchLabel, pitchSlider, juce::Slider::TextBoxBelow, "Pitch", " Hz", pitchLAF);
    juce::String freqID = "freq" + juce::String(stepIndex);
    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, freqID, pitchSlider);

    setSliderAndLabel(*this, toneLabel, toneSlider, juce::Slider::TextBoxBelow, "Tone", " %", toneLAF);
    juce::String toneID = "tone" + juce::String(stepIndex);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, toneID, toneSlider);

    setSliderAndLabel(*this, modLabel, modSlider, juce::Slider::NoTextBox, "P.Env", "", modLAF);
    juce::String modID = "mod" + juce::String(stepIndex);
    modAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, modID, modSlider);

    setSliderAndLabel(*this, repeatLabel, repeatSlider, juce::Slider::TextBoxBelow, "Repeat", "", repeatLAF);
    juce::String probID = "repeat" + juce::String(stepIndex);
    repeatAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, probID, repeatSlider);

    startTimerHz(30);
}

StepInterface::~StepInterface()
{
    pitchSlider.setLookAndFeel(nullptr);
    toneSlider.setLookAndFeel(nullptr);
    modSlider.setLookAndFeel(nullptr);
    repeatSlider.setLookAndFeel(nullptr);

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

    pitchLabel.setBounds(x, y + height * 0.035f, width, width/4);
    pitchSlider.setBounds(x, y + height * 0.035f, width, width);

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

DrumMainInterface::DrumMainInterface(MotherlyAudioProcessor& p) : audioProcessor(p)
{
    setSliderAndLabel(*this, tensionLabel, tensionSlider, juce::Slider::TextBoxBelow, "Tension", " %", tensionLAF);
    tensionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "tension", tensionSlider);

    setSliderAndLabel(*this, inharmLabel, inharmSlider, juce::Slider::TextBoxBelow, "Inharmoncity", " %", inharmLAF);
    inharmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "inharmonicity", inharmSlider);

    setSliderAndLabel(*this, positionLabel, positionSlider, juce::Slider::TextBoxBelow, "Position", " %", positionLAF);
    positionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    
    setSliderAndLabel(*this, op1Label, op1Slider, juce::Slider::NoTextBox, "Op 1", "", smallDialLAF);
    op1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op1Level", op1Slider);
    
    setSliderAndLabel(*this, op2Label, op2Slider, juce::Slider::NoTextBox, "Op 2", "", smallDialLAF);
    op2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op2Level", op2Slider);

    setSliderAndLabel(*this, op3Label, op3Slider, juce::Slider::NoTextBox, "Op 3", "", smallDialLAF);
    op3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "op3Level", op3Slider);

    setSliderAndLabel(*this, noiseLabel, noiseSlider, juce::Slider::NoTextBox, "Noise", "", smallDialLAF);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noiseLevel", noiseSlider);

    setSliderAndLabel(*this, noiseFreqLabel, noiseFreqSlider, juce::Slider::TextBoxBelow, "Freq", " % ", noiseFreqLAF);
    noiseFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noiseFreq", noiseFreqSlider);
    
    setSliderAndLabel(*this, outputLabel, outputSlider, juce::Slider::TextBoxBelow, "Output", " dB", outputLAF);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "outputGain", outputSlider);
    
    setSliderAndLabel(*this, algorithmLabel, algorithmSlider, juce::Slider::NoTextBox, "Algorithm", "", algorithmLAF);
    algorithmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "algorithm", algorithmSlider);
    
    addAndMakeVisible(stateButton);
    stateButton.setToggleable(true);
    stateButton.setClickingTogglesState(true);
    stateButton.setLookAndFeel(&stateLAF);
    stateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "active", stateButton);

}

DrumMainInterface::~DrumMainInterface()
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

void DrumMainInterface::paint(juce::Graphics& g)
{
    auto bg = getLocalBounds().toFloat();
    bg.reduce(5, 5);
    
    juce::Path bgFill;
    bgFill.addRoundedRectangle(bg, 5);
    
    g.setColour(Colours::Main::backgroundFill);
    g.fillPath(bgFill);
}

void DrumMainInterface::resized()
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

/*
void DrumMainInterface::setSliderAndLabel(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, juce::String suffix, UserInterfaceGraphics& lookAndFeel)
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
*/

