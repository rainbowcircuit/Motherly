#pragma once
#include <JuceHeader.h>
#include "DSP.h"

class Operator
{
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void resetPhase();
    void resetAngle();
    float processOperator();
    void setOperatorInputs(float frequency, double inPhase1, double inPhase2, double inPhase3, double inPhase4, float tone, float bias);
    
private:
    double sampleRate, operatorAngle = 0.0, operatorPhase = 0.0, inputPhase = 0.0;
    float fmAmount, softClipGain, softClipBias;
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

