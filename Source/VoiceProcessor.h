
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
    
    //==============================================================================
    
    void setStepParameters(int index, float pitchValue, float toneValue, float modValue, float probValue);
    void setGlobalParameters(float tensionValue, float inharmValue, float positionValue);
    void setVoiceLevels(float operatorLevelValue, float noiseLevelValue, float noiseFreqValue, float noiseBandwidthValue);
    void setAlgorithm(int algorithmValue);
    
    void setEnvelope()
    {
        float pitchFrom0to1 = pitchIn0to1 * 1170.0f + 30.0f;
        float tensionFrom0to1 = tensionIn0to1 * 19900.0f + 100.0f;
        ampEnvelope.setEnvelopeSlew(1.0f, 1000.0f/pitchFrom0to1 * tensionFrom0to1);
    }
 //   void setPatchBayParameters(int index, int inputValue);
 //   void selector(int outputIndex, int inputIndex, std::array<float, 8>& outputs, std::array<float, 12>& inputs, std::array<float, 12>& defaults);
    
    //==============================================================================
    
    void paramsIn0to1();
    void setDefaults();
    void overrideDefaults(int outputIndex, int inputIndex);
    void newParamsIn0to1();

private:
    // initial preparation
    double sampleRate = 0.0;
    bool isPrepared { false };
    
    // Step Sequencer
    int stepIndex = 0;

    // global parameters
    float gateValue = 0.0f;
    float tailInMs = 8000.0f; // global tail amount
    int algorithm = 0;
    
    // DSP Initialization
    std::array<Operator, 3> op;
    NoiseGenerator noise;
    
    LowPassGate ampEnvelope;
    CombFilter positionComb;
    
    // Synth Parameters
    std::array<float, 8> pitch = {}; // carrier pitch
    std::array<float, 8> tone = {}; // fm ratio and feedback
    std::array<float, 8> mod = {}; // carrier pitch mod amount
    std::array<float, 8> prob = {}; // carrier pitch mod amount

    juce::Random probGenerator;
    bool probIsTrue;
    
    float tension = 0.0f, inharmonicity = 0.0f, position = 0.0f;
    float operatorLevel = 1.0f, noiseLevel = 1.0f, noiseFreq = 1000.0f, noiseBandwidth = 10.0f;
    
    // outputs
    
    
    std::array<float, 8> outputsIn0to1;
    std::array<float, 12> inputsIn0to1;
    std::array<float, 12> defaultsIn0to1;
        
    float pitchIn0to1, toneIn0to1, tensionIn0to1, inharmIn0to1, positionInIn0to1, stepIn0to1, operLevelIn0to1, noiseLevelIn0to1, noiseFreqIn0to1, noiseBandIn0to1, algoIn0to1, vcaMixIn0to1;
    
    
    

    
    
};


class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

