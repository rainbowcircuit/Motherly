#include "FMOperator.h"

void Operator::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    combFilter.prepareToPlay(sampleRate, samplesPerBlock, numChannels);
}

void Operator::reset()
{
    operatorAngle = 0.0;
    operatorPhase = 0.0;
}

void Operator::setOperatorInputs(float frequency, double inputPhase, float tone, float position)
{
    operatorPhase = frequency/sampleRate;
    this->inputPhase = inputPhase;
    fmAmount = tone * 8.0f;
    combPosition = position;
    softClipGain = tone * 5.0f + 1.0f;
}

float Operator::processOperator()
{
    float modulator = inputPhase * fmAmount;
    
    // soft clip and comb filter
    combFilter.setDelayTime(combPosition);
    modulator = combFilter.processComb(std::tanh(modulator * softClipGain));
    
    float outputWaveform = std::sin(operatorAngle * juce::MathConstants<float>::pi * 2.0f + modulator);

    //increment and limit phase
    operatorAngle += operatorPhase;
    if (operatorAngle >= 1.0) operatorAngle -= 1.0;

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
    float filteredNoise = filter.processSample(noise, 2);
    
    return filteredNoise;
}
