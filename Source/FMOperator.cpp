#include "FMOperator.h"

void Operator::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    resetPhase();
    resetAngle();
}

void Operator::resetPhase()
{
    operatorPhase = 1.0;
}

void Operator::resetAngle()
{
    operatorAngle = 0.0;
}

void Operator::setOperatorInputs(float frequency, double inPhase1, double inPhase2, double inPhase3, double inPhase4, float tone, float bias)
{
    operatorPhase = frequency/sampleRate;
    inputPhase = inPhase1 + inPhase2 + inPhase3 + inPhase4;
    fmAmount = tone * 6.0f;
    softClipGain = tone * 1.5f + 1.0f;
    softClipBias = 1.0f - bias;
}

float Operator::processOperator()
{
    float modulator = inputPhase * fmAmount;
    modulator = std::tanh(modulator * softClipGain + softClipBias);
    float phase = operatorAngle * juce::MathConstants<float>::twoPi + modulator;
    float outputWaveform = std::sin(phase);
    
    //increment and limit phase
    operatorAngle += operatorPhase;
    if (operatorAngle >= 1.0f) operatorAngle -= 1.0f;

    // output
    return outputWaveform;
}

void NoiseGenerator::prepareToPlay(double sampleRate)
{
    filter.setSampleRate(sampleRate);
    filter.reset();
}

void NoiseGenerator::setFilter(float frequency, float Q)
{
    filter.setCoefficients(frequency, Q);
    
}

float NoiseGenerator::processNoiseGenerator()
{
    float noise = noiseSource.nextFloat();
    float filteredNoise = filter.processSample(noise, 1);

    return filteredNoise;
}
