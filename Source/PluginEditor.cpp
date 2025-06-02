
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

    addAndMakeVisible(*mainInterface);
    addAndMakeVisible(*patchBayInterface);
    addAndMakeVisible(*presetInterface);
    

    for (int step = 0; step < 8; step++)
    {
        stepInterface[step] = std::make_unique<StepInterface>(audioProcessor, step);
        addAndMakeVisible(*stepInterface[step]);
    }
    
    startTimerHz(30);
    setSize (775, 475);
}

MotherlyAudioProcessorEditor::~MotherlyAudioProcessorEditor()
{
}

//==============================================================================
void MotherlyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::InterfaceMain::backgroundFillAlt);
    
    juce::Path stepDot;

    for (int step = 0; step < 8; step++)
    {
        stepInterface[step]->setIsStepIndex(stepIndexAtomic);
       // stepInterface[step]->setAmplitude(amplitudeAtomic);
    }
}

void MotherlyAudioProcessorEditor::resized()
{
    mainInterface->setBounds(0, 350, 600, 125);
    for (int step = 0; step < 8; step++)
    {
        int xIncrement = step * 75;
        stepInterface[step]->setBounds(xIncrement, 50, 75, 300);
    }
    
    patchBayInterface->setBounds(600, 50, 175, 300);
    patchBayInterface->resized();
    
    presetInterface->setBounds(5, 5, 600, 50);
}

void MotherlyAudioProcessorEditor::timerCallback()
{
    stepIndexAtomic = stepIndexAtomic.load();
    amplitudeAtomic = amplitudeAtomic.load();
    repaint();
}
