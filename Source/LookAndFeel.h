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
    namespace InterfaceMain
    {
    const juce::Colour backgroundFill = { 35, 32, 34 };
    const juce::Colour backgroundHoverFill = { 55, 52, 54 };
    const juce::Colour backgroundFillAlt = { 45, 42, 44 };
    const juce::Colour textColor = { 115, 117, 116 };
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

    
    
        const std::array<juce::Colour, 8> stepMainColour2 = {
            juce::Colour { 219, 153, 147 }, // Top: light rosy
            juce::Colour { 208, 144, 127 },
            juce::Colour { 204, 147, 114 },
            juce::Colour { 195, 150, 110 }, // Desaturated orange
            juce::Colour { 183, 152, 96 },
            juce::Colour { 173, 155, 103 },
            juce::Colour { 166, 157, 114 },
            juce::Colour { 161, 160, 124 }  // Bottom: light earthy gray-green
        };

        const std::array<juce::Colour, 8> stepMainColourAlt = {
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
}
