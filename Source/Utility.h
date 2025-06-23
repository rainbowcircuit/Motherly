#pragma once
#include <JuceHeader.h>

namespace Utility {
    static inline float scale (float input, float inputMin, float inputMax, float outputMin, float outputMax) {
        if (inputMin == inputMax) {
            return outputMin;
        }
        
        // Clamp input to the input range
        if (input < inputMin) {
            input = inputMin;
        }
        if (input > inputMax) {
            input = inputMax;
        }
        
        float normalizedPosition = (input - inputMin) / (inputMax - inputMin);
        float output = outputMin + normalizedPosition * (outputMax - outputMin);
        
        return output;
    }

    static inline float scaleLog(float value, float min, float max, float curve) {
        // Ensure min is positive (required for logarithmic scale)
        value = std::clamp(value, 0.0f, 1.0f);
        min = std::max(min, 0.0001f); // Avoid zero or negative min

        float curvedX = std::pow(value, curve);
        return min * std::pow(max / min, curvedX);
    }

    static inline float scaleExp(float value, float min, float max, float curve)
    {
        value = std::clamp(value, 0.0f, 1.0f);
        double curvedX = std::pow(value, curve);
        return min + (max - min) * curvedX;
    }

    static inline float getModWheelValue(juce::MidiBuffer& midiBuffer, float lastValue)
    {
        float value = lastValue;
        for (const juce::MidiMessageMetadata metadata : midiBuffer)
        {
            auto message = metadata.getMessage();
            
            if (message.isControllerOfType(1)) {
                value = message.getControllerValue();
            }
        }
        midiBuffer.clear();
        return value;
    }
}

class DemoMode
{
public:
    // use this at the processblock for demo mode.
    void prepareToPlay(double sampleRate)
    {
        increment = 1.0f/(sampleRate * 60);
        phase = 0.0f;
    }

    void processDemoMode(juce::AudioBuffer<float> &buffer)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                phase += increment;
                if (phase >= 1.0f) { phase = 0.0f; }

                if (phase >= 0.8f) {
                    channelData[sample] = rand.nextFloat() * 0.03125;
                } else {
                    channelData[sample] = channelData[sample];
                }
            }
        }
    }
    
    float increment, phase;
    juce::Random rand;
};
