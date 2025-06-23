#include "DSP.h"

void Metro::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void Metro::reset()
{
    counterAccum = 0.0;
}

void Metro::setRate(int rate)
{
    this->rate = rate;
}

void Metro::setRepeat(int index, int repeatValue)
{
    repeat[index] = repeatValue;
}

void Metro::setStepIndex(int stepIndex)
{
    this->stepIndex = stepIndex;
}

float Metro::getGate(bool getBaseRate)
{
    double bpmToHz = (bpm/60.0) * subdivisionMultiplier[rate];
    
    float gateSize = 0.025;
    int repeatIndex = repeat[stepIndex];
    if (getBaseRate) repeatIndex = 0;
    
    float gate = 0.0f;
    if (isPlaying){
        counterAccum += bpmToHz/sampleRate;
        if (counterAccum >= 1.0) counterAccum -= 1.0;
        
        switch(repeatIndex){
            case 0: {
                gate = counterAccum >= 0.0 && counterAccum < 0.0 + gateSize;
                break;
                
            } case 1: { // repeat 2
                bool gateBool1 = counterAccum >= 0.0 && counterAccum < 0.0 + gateSize;
                bool gateBool2 = counterAccum >= 0.5 && counterAccum < 0.5 + gateSize;
                gate = gateBool1 || gateBool2;
                break;
                
            } case 2: { // repeat 3
                bool gateBool1 = counterAccum >= 0.0 && counterAccum < 0.0 + gateSize;
                bool gateBool2 = counterAccum >= 0.333 && counterAccum < 0.333 + gateSize;
                bool gateBool3 = counterAccum >= 0.666 && counterAccum < 0.666 + gateSize;
                gate = gateBool1 || gateBool2 || gateBool3;
                break;
                
            } case 3: { // repeat 4
                bool gateBool1 = counterAccum >= 0.0 && counterAccum < 0.0 + gateSize;
                bool gateBool2 = counterAccum >= 0.25 && counterAccum < 0.25 + gateSize;
                bool gateBool3 = counterAccum >= 0.5 && counterAccum < 0.5 + gateSize;
                bool gateBool4 = counterAccum >= 0.75 && counterAccum < 0.75 + gateSize;
                gate = gateBool1 || gateBool2 || gateBool3 || gateBool4;
                break;
            }
        }
    } else {
        reset();
        gate = 0.0f;
    }
    
    return gate;
}

void Metro::getTransport(juce::AudioPlayHead* playhead)
{
    if (playhead == nullptr){ return; }
    
    const auto opt = playhead->getPosition();
    const auto& pos = *opt;

    if (!opt.hasValue()) { return; }

    if (pos.getBpm().hasValue()) {
        bpm = *pos.getBpm();
    }
    
    // get transport state
    isPlaying = pos.getIsPlaying();
    if (!isPlaying)
    {
        reset();
    }
}

bool Metro::getIsPlaying()
{
    return isPlaying;
}

void LowPassGate::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void LowPassGate::setEnvelopeSlew(float riseInMilliseconds, float fallInMilliseconds)
{
    envelopeRise = (riseInMilliseconds/1000.0f) * sampleRate;
    envelopeFall = (fallInMilliseconds/1000.0f) * sampleRate;
}

void LowPassGate::triggerEnvelope(float gateValue)
{
    gate = gateValue;
}


float LowPassGate::generateEnvelope(float gate)
{
    bool gateThreshold = gate != 0.0f;
    
    float segmentSelect = gateThreshold ? envelopeRise : envelopeFall;
    segmentSelect = std::max(segmentSelect, 1.0f);
    
    float segmentTimeT60 = processT60(segmentSelect);
    envelope = (1.0f - segmentTimeT60) * gate + segmentTimeT60 * unitDelay;
    unitDelay = envelope;
    return envelope;
}

bool LowPassGate::isActive()
{
    return envelope > 0.0001f;
}

float LowPassGate::processT60(float input)
{
    
    float t60 = std::exp(safediv(-6.9077552789821f, input));
    return t60;
}

float LowPassGate::safediv(float numerator, float denominator)
{
    return denominator != 0.0f ? numerator / denominator : numerator / epsilon;
}

void SVF::setSampleRate(float sampleRate)
{
    this->sampleRate = sampleRate;
}

void SVF::setCoefficients(float freq, float Q)
{
    g = fastTan.tan(M_PI * freq / sampleRate);
    k = 1.0f / Q;
    a1 = 1.0f / (1.0f + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
}

void SVF::reset()
{
    // coefficients
    g = 0.0f;
    k = 0.0f;
    a1 = 0.0f;
    a2 = 0.0f;
    a3 = 0.0f;
    
    // unit delays
    z1 = 0.0f;
    z2 = 0.0f;
}

float SVF::processSample(float input, int filterType) noexcept
{
    float v3 = input - z2;
    float v1 = a1 * z1 + a2 * v3;
    float v2 = z2 + a2 * z1 + a3 * v3;
    
    // update unit delay
    z1 = 2.0f * v1 - z1;
    z2 = 2.0f * v2 - z2;
    
    float filterOutput;
    switch(filterType){
        case 1: // LP
            filterOutput = v2;
            break;
            
        case 2: // BP
            filterOutput = v1 * k;
            break;
            
        case 3: // HP
            filterOutput = input + -k * v1 * -v2;
            break;
    }
    return filterOutput;
}


