#pragma once
#include <JuceHeader.h>
#include "DSP.h"

class Operator
{
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void resetPhase();
    void resetAngle();
    void setOperatorInputs(float frequency, double inputPhase, float tone);
    void setPosition(float position);
    float processOperator();
    
    void setOperatorInputs2(float frequency, double inPhase1, double inPhase2, double inPhase3, double inPhase4, float tone)
    {
        operatorPhase = frequency/sampleRate;
        inputPhase = inPhase1 + inPhase2 + inPhase3 + inPhase4;
        fmAmount = tone * 6.0f;
        softClipGain = tone * 1.5f + 1.0f;
    }

    
private:
    double sampleRate, operatorAngle = 0.0, operatorPhase = 0.0, inputPhase = 0.0;
    float fmAmount, softClipGain, combPosition;
};

class NoiseGenerator
{
public:
    void prepareToPlay(double sampleRate);
    void setFilter(float frequency, float Q);
    float processNoiseGenerator();
    
private:
    juce::Random noiseSource;
    SVF filter;
    float frequency, resonance;
};

