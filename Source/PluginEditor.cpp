
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PatchBayGraphics.h"
#include "UserInterfaceLayout.h"

//==============================================================================
MotherlyAudioProcessorEditor::MotherlyAudioProcessorEditor (MotherlyAudioProcessor& p, std::atomic<int>& stepIndexAtomic_, std::atomic<float>& amplitudeAtomic_) : AudioProcessorEditor (&p), stepIndexAtomic(stepIndexAtomic_), amplitudeAtomic(amplitudeAtomic_), audioProcessor (p)
{
    mainInterface = std::make_unique<DrumMainInterface>(audioProcessor);
    patchBayInterface = std::make_unique<PatchBay>(audioProcessor);
    presetInterface = std::make_unique<PresetInterface>(audioProcessor, audioProcessor.apvts);
    patchBayInterface->refreshFromParameters();

    addAndMakeVisible(*mainInterface);
    addAndMakeVisible(*patchBayInterface);
    addAndMakeVisible(*presetInterface);
    
    for (int step = 0; step < 8; step++)
    {
        stepInterface[step] = std::make_unique<StepInterface>(audioProcessor, step);
        addAndMakeVisible(*stepInterface[step]);
    }
    
    startTimerHz(30);
    setSize (780, 485);
}

MotherlyAudioProcessorEditor::~MotherlyAudioProcessorEditor()
{
}

//==============================================================================
void MotherlyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::Main::backgroundFillAlt);
    
    juce::Path stepDot;

    for (int step = 0; step < 8; step++)
    {
        stepInterface[step]->setIsStepIndex(stepIndexAtomic);
    }
}

void MotherlyAudioProcessorEditor::resized()
{
    presetInterface->setBounds(5, 5, 765, 50);

    for (int step = 0; step < 8; step++)
    {
        int xIncrement = step * 75;
        stepInterface[step]->setBounds(5 + xIncrement, 55, 75, 300);
    }
    
    patchBayInterface->setBounds(605, 55, 170, 300);
    patchBayInterface->resized();
    mainInterface->setBounds(5, 355, 770, 125);
}

void MotherlyAudioProcessorEditor::timerCallback()
{
    stepIndexAtomic = stepIndexAtomic.load();
    amplitudeAtomic = amplitudeAtomic.load();
    repaint();
}
