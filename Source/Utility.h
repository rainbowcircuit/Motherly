#pragma once

namespace Utility {
    static float scale (float input, float inputMin, float inputMax, float outputMin, float outputMax) {
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

    static float scaleLog(float value, float min, float max, float curve) {
        // Ensure min is positive (required for logarithmic scale)
        value = std::clamp(value, 0.0f, 1.0f);
        min = std::max(min, 0.0001f); // Avoid zero or negative min

        float curvedX = std::pow(value, curve);
        return min * std::pow(max / min, curvedX);
    }

    static float scaleExp(float value, float min, float max, float curve)
    {
        value = std::clamp(value, 0.0f, 1.0f);
        double curvedX = std::pow(value, curve);
        return min + (max - min) * curvedX;
    }
}

