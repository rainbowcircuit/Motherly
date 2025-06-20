#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PatchBayGraphics.h"
#include "UserInterfaceLayout.h"

class MotherlyAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    MotherlyAudioProcessorEditor (MotherlyAudioProcessor& p, std::atomic<int>& stepIndexAtomic, std::atomic<float>& amplitudeAtomic);
    ~MotherlyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    
private:
    void timerCallback() override;

    std::atomic<int>& stepIndexAtomic;
    std::atomic<float>& amplitudeAtomic;

    std::unique_ptr<PresetInterface> presetInterface;
    std::unique_ptr<DrumMainInterface> mainInterface;
    std::unique_ptr<PatchBay> patchBayInterface;
    std::array<std::unique_ptr<StepInterface>, 8> stepInterface;
    
    MotherlyAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MotherlyAudioProcessorEditor)
};
