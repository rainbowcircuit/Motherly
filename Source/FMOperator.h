/*
  ==============================================================================

    FMOperator.h
    Created: 1 Jun 2025 6:18:15pm
    Author:  Takuma Matsui

  ==============================================================================
*/

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

