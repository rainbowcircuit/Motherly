#include <JuceHeader.h>
#pragma once

namespace Colours
{
    namespace Main
    {
    const juce::Colour backgroundFill = { 37, 35, 37 };//35 32 34
    const juce::Colour backgroundHoverFill = { 42, 40, 42 };
    const juce::Colour backgroundActiveFill = { 47, 45, 47 };

    const juce::Colour backgroundFillAlt = { 32, 30, 32 };
    const juce::Colour textColor = { 115, 117, 116 };
    
    const juce::Colour iconWhite = { 190, 192, 191 };
    const juce::Colour iconWhiteHover = { 200, 202, 201 };
    const juce::Colour iconLightGrey = { 150, 152, 151 };
    const juce::Colour iconDarkGrey = { 70, 72, 71 };
    const juce::Colour iconBlack = { 25, 22, 24 };
    }

    namespace Gradient
    {
        const std::array<juce::Colour, 8> gradient = {
            juce::Colour { 219, 153, 147 },
            juce::Colour { 208, 144, 127 },
            juce::Colour { 204, 147, 114 },
            juce::Colour { 195, 150, 110 },
            juce::Colour { 183, 152, 96 },
            juce::Colour { 173, 155, 103 },
            juce::Colour { 166, 157, 114 },
            juce::Colour { 161, 160, 124 }
        };

        const std::array<juce::Colour, 8> gradientHover = {
            juce::Colour { 229, 163, 157 },
            juce::Colour { 218, 154, 137 },
            juce::Colour { 214, 157, 124 },
            juce::Colour { 205, 160, 120 },
            juce::Colour { 193, 162, 106 },
            juce::Colour { 183, 165, 113 },
            juce::Colour { 176, 167, 124 },
            juce::Colour { 171, 170, 134 }
        };
    
        const std::array<juce::Colour, 8> gradientDarker = {
            juce::Colour { 209, 143, 137 },
            juce::Colour { 198, 134, 117 },
            juce::Colour { 194, 137, 104 },
            juce::Colour { 185, 140, 100 },
            juce::Colour { 173, 142, 86 },
            juce::Colour { 163, 145, 93 },
            juce::Colour { 156, 147, 104 },
            juce::Colour { 151, 150, 114 }
        };
    }
}
