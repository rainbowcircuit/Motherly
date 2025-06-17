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

void Operator::setOperatorInputs(float frequency, double inputPhase, float tone)
{
    operatorPhase = frequency/sampleRate;
    this->inputPhase = inputPhase;
    fmAmount = tone * 6.0f;
    softClipGain = tone * 1.5f + 1.0f;
}

inline float wrapToMinusPiToPi(float x)
{
    constexpr float twoPi = juce::MathConstants<float>::twoPi;
    constexpr float pi = juce::MathConstants<float>::pi;
    x += pi;
    x -= twoPi * std::floor(x / twoPi);
    return x - pi;
}

float Operator::processOperator()
{
    float modulator = inputPhase * fmAmount;
    modulator = std::tanh(modulator * softClipGain);
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
