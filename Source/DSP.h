#pragma once
#include <JuceHeader.h>

class CombFilter
{
public:
    void prepareToPlay(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels);
    void setDelayTime(float delayTime);
    float processComb(float input);
    
private:
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> combDelay;
    float delayTime = 48000.0f;
    double sampleRate = 0.0;
};

class ImpulseMetro
{
public:
    // tempo synced sample accurate counter that runs on the audio thread.
    void setSampleRate(double sampleRate);
    void reset();
    
    void setRate(int rate);
    bool getImpulse();
    
    void getTransport(juce::AudioPlayHead* playhead);
    bool getIsPlaying();
    
private:
    double sampleRate;
    double counterAccum = 0.0, previousCounterAccum = 0.0;
    double bpm = 120.0;
    int rate;
    bool isPlaying = false;
    
    std::array <double, 7> subdivisionMultiplier =
     {
         0.5,
         2.0 / 3.0,
         1.0,
         4.0 / 3.0,
         2.0,
         8.0 / 3.0,
         4.0
     };
};

class LowPassGate
{
public:
    void setSampleRate(double sampleRate);
    void setEnvelopeSlew(float riseInMilliseconds, float fallInMilliseconds);
    
    void noteOn();
    void noteOff();
    
    float generateEnvelope();
    bool isActive();
    
private:
    float processT60(float input);
    float safediv(float numerator, float denominator);
    
    int count = 0;
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

