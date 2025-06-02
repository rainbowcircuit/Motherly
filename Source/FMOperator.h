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
/*
 Operator in the loosest sense -- with a comb filter and diode distortion.
 Since the EG is shared, it is handled externally in synth voice.
 */
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();
    void setOperatorInputs(float frequency, double inputPhase, float fmAmount);
    float processOperator();
    
private:
    CombFilter combFilter;
    double sampleRate, operatorAngle = 0.0, operatorPhase = 0.0, inputPhase = 0.0;
    float fmAmount, softClipGain;
         
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
