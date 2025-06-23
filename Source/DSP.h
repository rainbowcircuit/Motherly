#pragma once
#include <JuceHeader.h>

class Metro
{
public:
    void setSampleRate(double sampleRate);
    void reset();
    
    void setRate(int rate);
    void setRepeat(int index, int repeatValue);
    void setStepIndex(int stepIndex);
    float getGate(bool getBaseRate);
    void getTransport(juce::AudioPlayHead* playhead);
    bool getIsPlaying();
    
private:
    double bpm = 120.0, sampleRate, counterAccum = 0.0;
    int rate, stepIndex = 7;
    bool isPlaying = false;
    
    std::array <double, 7> subdivisionMultiplier =
     {
         0.25,
         1.0 / 3.0,
         0.5,
         2.0 / 3.0,
         1.0,
         4.0 / 3.0,
         2.0,
     };
    std::array<int, 8> repeat = {};
    
};

class LowPassGate
{
public:
    void setSampleRate(double sampleRate);
    void setEnvelopeSlew(float riseInMilliseconds, float fallInMilliseconds);

    void triggerEnvelope(float gate);
    
    float generateEnvelope(float gate);
    bool isActive();
    
private:
    float processT60(float input);
    float safediv(float numerator, float denominator);
    
    int count = 0, gateHoldCounter = 0;
    double sampleRate = 0.0;
    float unitDelay = 0.0f;
    static constexpr float epsilon = 1e-6f; // safety for dividing by 0
    float envelopeRise = 0.0f, envelopeFall = 0.0f;
    float gate = 0.0f, lastGate = 0.0f, envelope = 0.0f;
};

class SVF
{
public:
    void setSampleRate(float sampleRate);
    void setCoefficients(float freq, float Q);
    float processSample(float input, int filterType) noexcept;
    void reset();

private:
    float sampleRate = 48000.0f;
    float cutoff;
    float resonance;
    
    // coefficients
    float g, k, a1, a2, a3;
    // unit delays
    float z1, z2;
    juce::dsp::FastMathApproximations fastTan;
};

