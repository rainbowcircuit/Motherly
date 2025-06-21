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
    
    struct VoiceParams
    {
        float envelope, modEnvelope, pitch, tone, inharm;
        int algorithm;
        float op0Level, op1Level, op2Level, noiseLevel;
    };
    
    SynthVoice::VoiceParams processParametersPerBlock(float numSamples);
    void processParametersPerSample(SynthVoice::VoiceParams &params);
    float processSynthVoice(SynthVoice::VoiceParams params);

    //==============================================================================
    
    int stepIndex = 0;
    int getStepIndex();
    void setSequencer(juce::AudioPlayHead &p, int rate, bool active);
    void setStepParameters(int index, float pitchValue, float toneValue, float modValue, float repeatValue);
    
    void setGlobalParameters(float tensionValue, float inharmValue, float positionValue);
    void setVoiceLevels(float outputGainValue, float op0LevelValue, float op1LevelValue, float op2LevelValue, float noiseLevelValue, float noiseFreqValue);
    void setModWheel(float modWheel);
    
    //==============================================================================

    void setEnvelope();
    void onTrigger(float gate);
    float previousGate = 0.0f;
    float generateRand();
    
    //==============================================================================
    float operator0, operator1, operator2;
    std::array<float, 4> op2Gain;
    std::array<float, 4> op1Gain;
    std::array<float, 4> op0Gain;
    std::array<float, 4> outGain;
    
    std::array<juce::SmoothedValue<float>, 4> op2GainInterpolation;
    std::array<juce::SmoothedValue<float>, 4> op1GainInterpolation;
    std::array<juce::SmoothedValue<float>, 4> op0GainInterpolation;
    std::array<juce::SmoothedValue<float>, 4> outGainInterpolation;

    void setAlgorithm(int algorithmValue);
    void setAlgorithmGain(int algorithmIndex);
    int algorithmRawValue;

    //==============================================================================
    
    void paramsIn0to1();
    void setDefaults();
    void overrideDefaults(int outputIndex, int inputIndex, bool outputInvertIndex);
    void newParamsIn0to1();

private:
    bool active;
    // initial preparation
    double sampleRate = 0.0;
    bool isPrepared { false };
    
    // DSP Objects
    std::array<Operator, 3> op;
    NoiseGenerator ns;
    StepSequencer seq;
    juce::Random rand;

    LowPassGate ampEnvelope;
    CombFilter combFilter;
    
    // Parameters
    std::array<float, 9> outputsIn0to1;
    std::array<float, 11> inputsIn0to1;
    std::array<float, 11> defaultsIn0to1;

    std::array<float, 8> pitchRawValues = {};
    std::array<float, 8> toneRawValues = {};
    std::array<float, 8> modRawValues = {};
    std::array<float, 8> repeatRawValues = {};

    float envRawValue =      0.0f,
    randRawValue =           0.0f,
    tensionRawValue =        0.0f,
    inharmonicityRawValue =  0.0f,
    positionRawValue =       0.0f,
    operatorLevelRawValue =  1.0f,
    vcaSignalRawValue =      0.0f,
    noiseSignalRawValue =    0.0f,
    noiseLevelRawValue =     1.0f,
    noiseFreqRawValue =      1000.0f,
    op0LevelRawValue =       0.0f,
    op1LevelRawValue =       0.0f,
    op2LevelRawValue =       0.0f,
    modWheelRawValue =       0.0f,
    outputGainRawValue =     0.0f;
    
    float pitchIn0to1,
    toneIn0to1,
    tensionIn0to1,
    inharmIn0to1,
    positionIn0to1,
    algoIn0to1,
    operLevelIn0to1,
    noiseLevelIn0to1,
    noiseFreqIn0to1,
    op0In0to1,
    op1In0to1,
    op2In0to1;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pitchSmooth,
    toneSmooth,
    inharmSmooth,
    positionSmooth,
    outputSmooth,
    op0LevelSmooth,
    op1LevelSmooth,
    op2LevelSmooth,
    noiseLevelSmooth,
    noiseFreqSmooth;
};

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

