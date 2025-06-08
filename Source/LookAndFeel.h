/*
  ==============================================================================

    LookAndFeel.h
    Created: 25 Feb 2025 5:04:52pm
    Author:  Takuma Matsui

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once

namespace Colours
{
    namespace Main
    {
    const juce::Colour backgroundFill = { 35, 32, 34 };
    const juce::Colour backgroundHoverFill = { 40, 37, 39 };
    const juce::Colour backgroundActiveFill = { 45, 42, 44 };

    const juce::Colour backgroundFillAlt = { 30, 27, 29 };
    const juce::Colour textColor = { 115, 117, 116 };
    
    const juce::Colour iconWhite = { 190, 192, 191 };
    const juce::Colour iconWhiteHover = { 200, 202, 201 };
    const juce::Colour iconLightGrey = { 150, 152, 151 };
    const juce::Colour iconDarkGrey = { 80, 82, 81 };

    }

    namespace StepColour
    {
        const juce::Colour iconWhite = { 190, 192, 191 };
        const juce::Colour iconWhiteAlt = { 190, 192, 191 };
        const juce::Colour iconShadow = { 150, 152, 151 };

    
    const std::array<juce::Colour, 8> stepMainColour = {
        juce::Colour { 219, 153, 147 }, // Top: light rosy
        juce::Colour { 208, 144, 127 },
        juce::Colour { 204, 147, 114 },
        juce::Colour { 195, 150, 110 }, // Desaturated orange
        juce::Colour { 183, 152, 96 },
        juce::Colour { 173, 155, 103 },
        juce::Colour { 166, 157, 114 },
        juce::Colour { 161, 160, 124 }  // Bottom: light earthy gray-green
    };

    
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
    }

}
