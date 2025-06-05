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

class PatchBayHelper
{
public:
    struct patchBayValues {
        float valueIn0to1;
        int localInputIndex;
        int localOutputIndex;
    };
    
    std::array<patchBayValues, 20> patchPointLayout {{
        { 0.0f, 0, 1 }, // pitch out
        { 0.0f, 0, 2 }, // tone out
        { 0.0f, 0, 3 }, // mod out
        { 0.0f, 0, 4 }, // prob out
        
        { 0.0f, 1, 0 }, // pitch in
        { 0.0f, 2, 0 }, // tone in
        { 0.0f, 0, 5 }, // eg out
        { 0.0f, 0, 6 }, // step out

        { 0.0f, 3, 0 }, // tension in
        { 0.0f, 4, 0 }, // inharm in
        { 0.0f, 5, 0 }, // position in
        { 0.0f, 6, 0 }, // algo in

        { 0.0f, 7, 0 }, // operator level in
        { 0.0f, 8, 0 }, // noise level in
        { 0.0f, 9, 0 }, // noise freq in
        { 0.0f, 10, 0 }, // noise bwidth in

        { 0.0f, 11, 0 }, //
        { 0.0f, 12, 0 }, // vca
        { 0.0f, 0, 7 }, // mod wheel out
        { 0.0f, 0, 8 }, // pbend out
    }};

    int getLocalIndex(int indexToFind, bool findInput)
    {
        for (int point = 0; point < patchPointLayout.size(); ++point)
        {
            if (findInput){
                if (patchPointLayout[point].localInputIndex == indexToFind)
                {
                    return point;
                }
            } else {
                if (patchPointLayout[point].localOutputIndex == indexToFind)
                {
                    return point;
                }
            }
        }
    }
    
    float outputSelector(int outputIndex, int inputValue, float defaultValue, std::array<float, 8> outputs)
    {
        if (inputValue == 0) return defaultValue;
        return outputs[outputIndex];
    }
};
