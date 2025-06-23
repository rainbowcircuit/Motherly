#pragma once
#include <JuceHeader.h>
#include "StepSequencer.h"
#include "VoiceProcessor.h"

//==============================================================================
/**
*/
class MotherlyAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MotherlyAudioProcessor();
    ~MotherlyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts {
        *this, nullptr, "Parameters", createParameterLayout()
    };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    float lastModWheel;

    std::atomic<int> stepIndexAtomic;
    std::atomic<float> amplitudeAtomic;
    juce::Synthesiser synth;
    DemoMode demoMode;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MotherlyAudioProcessor)
};
