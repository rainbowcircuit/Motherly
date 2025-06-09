
#pragma once
#include <JuceHeader.h>
#include "DSP.h"
#include "FMOperator.h"
#include "StepSequencer.h"
#include "Utility.h"



class SynthVoice : public juce::SynthesiserVoice
{
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples) override;
    
    //==============================================================================
    
    void setStepIndex(int index);
    void setStepParameters(int index, float pitchValue, float toneValue, float modValue, float repeatValue);
    void setGlobalParameters(float tensionValue, float inharmValue, float positionValue);
    void setVoiceLevels(float outputGainValue, float op0LevelValue, float op1LevelValue, float op2LevelValue, float noiseLevelValue, float noiseFreqValue);
    
    //==============================================================================

    void setEnvelope();
    void triggerEnvelope(float gate);

    //==============================================================================
    
    void setAlgorithm(int algorithmValue);
    
    struct VoiceParams
    {
        float envelope, modEnvelope, pitch, tone, inharm, position, op0Level, op1Level, op2Level, operLevel, noiseLevel;
    };

    float processAlgorithm0(VoiceParams p);
    float processAlgorithm1(VoiceParams p);
    float processAlgorithm2(VoiceParams p);
    float processAlgorithm3(VoiceParams p);
    float processAlgorithm4(VoiceParams p);
    float processAlgorithm5(VoiceParams p);
    float processAlgorithm6(VoiceParams p);
    float processAlgorithm7(VoiceParams p);
    float processAlgorithm8(VoiceParams p);
    float processAlgorithm9(VoiceParams p);

    //==============================================================================
    
    void paramsIn0to1();
    void setDefaults();
    void overrideDefaults(int outputIndex, int inputIndex);
    void newParamsIn0to1();

private:
    // initial preparation
    double sampleRate = 0.0;
    bool isPrepared { false };
    
    // step Sequencer
    int stepIndex = 0;

    // global parameters
    float previousGate = 0.0f;
    float tailInMs = 8000.0f; // global tail amount
    
    // DSP Initialization
    std::array<Operator, 3> op;
    NoiseGenerator ns;
    
    LowPassGate ampEnvelope;
    
    // Synth Parameters
    std::array<float, 8> pitchRawValues = {}; // carrier pitch
    std::array<float, 8> toneRawValues = {}; // fm ratio and feedback
    std::array<float, 8> modRawValues = {}; // carrier pitch mod amount
    std::array<float, 8> repeatRawValues = {}; // carrier pitch mod amount

    float tensionRawValue = 0.0f, inharmonicityRawValue = 0.0f, positionRawValue = 0.0f, operatorLevelRawValue = 1.0f, op0LevelRawValue = 1.0f, op1LevelRawValue = 1.0f, op2LevelRawValue = 1.0f, noiseLevelRawValue = 1.0f, noiseFreqRawValue = 1000.0f, noiseBandwidthRawValue = 10.0f, pitchWheelRawValue = 0.0f, modWheelRawValue = 0.0f, outputGainRawValue = 0.0f;

    int algorithmRawValue = 0;

    // patchbay
    std::array<float, 8> outputsIn0to1;
    std::array<float, 12> inputsIn0to1;
    std::array<float, 12> defaultsIn0to1;
        
    float pitchIn0to1, toneIn0to1, tensionIn0to1, inharmIn0to1, positionInIn0to1, stepIn0to1, operLevelIn0to1, noiseLevelIn0to1, noiseFreqIn0to1, noiseBandIn0to1, algoIn0to1, vcaMixIn0to1;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pitchSmooth, toneSmooth, tensionSmooth, inharmSmooth, positionSmooth, outputSmooth, op0LevelSmooth, op1LevelSmooth, op2LevelSmooth, noiseLevelSmooth, noiseFreqSmooth;
};


class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

