
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
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    //==============================================================================
    
    void setStepIndex(int index);
    void renderOscillator(juce::AudioBuffer<float> buffer);
    
    //==============================================================================
    
    void setStepParameters(int index, float pitchValue, float toneValue, float modValue, float probValue);
    void setGlobalParameters(float tensionValue, float inharmValue, float positionValue);
    void setVoiceLevels(float operatorLevelValue, float noiseLevelValue, float noiseFreqValue, float noiseBandwidthValue);
    void setAlgorithm(int algorithmValue);
    
    
    void setEnvelope()
    {
        ampEnvelope.setEnvelopeSlew(1.0f, 1000.0f/pitch[stepIndex] * (tension * 5.0f));
    }
    void setPatchBayParameters(int index, int inputValue);
    
    
    float selector(int index, float in0, float in1, float in2, float in3, float in4, float in5, float in6)
    {
        if (index == 1) { return in1; }
        else if (index == 2) { return in2; }
        else if (index == 3) { return in3; }
        else if (index == 4) { return in4; }
        else if (index == 5) { return in5; }
        else if (index == 6) { return in6; }
        else { return in0; }
    }
    
private:
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
};

class SynthSound : public juce::SynthesiserSound
{
public:
    
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
    
private:
};

