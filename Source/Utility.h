#pragma once

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

    static inline float getModWheelValue(juce::MidiBuffer& midiBuffer)
    {
        float value = 0.0f;
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


