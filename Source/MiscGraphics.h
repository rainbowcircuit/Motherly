/*
  ==============================================================================

    MiscGraphics.h
    Created: 1 Jun 2025 6:06:21pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class ButtonGraphics : public juce::LookAndFeel_V4
{
public:
    ButtonGraphics(int graphicIndex);
    
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawSaveButton(juce::Graphics& g, float x, float y, float width, float height)
    {
        float graphicMargin = width * 0.3f;
        float graphicSize = width * 0.4f;
        x = x + graphicMargin;
        y = y + graphicMargin;

        juce::Point<float> topLeft { x, y };
        juce::Point<float> slopeStart { x + graphicSize * 0.75f, y };
        juce::Point<float> slopeEnd { x + graphicSize, y + graphicSize * 0.25f};
        juce::Point<float> botRight { x + graphicSize, y + graphicSize };
        juce::Point<float> botLeft { x, y + graphicSize };

        juce::Path bodyPath;
        bodyPath.startNewSubPath(topLeft);
        bodyPath.lineTo(slopeStart);
        bodyPath.lineTo(slopeEnd);
        bodyPath.lineTo(botRight);
        bodyPath.lineTo(botLeft);
        bodyPath.closeSubPath();
        bodyPath = bodyPath.createPathWithRoundedCorners(1.0f);
        g.setColour(Colours::InterfaceMain::textColor);
        g.fillPath(bodyPath);

    }

    
    void drawArrowButton(juce::Graphics& g, float x, float y, float width, float height, bool isLeftArrow)
    {
        float graphicMargin = width * 0.4f;
        float graphicSize = width * 0.2f;
        x = x + graphicMargin;
        y = y + graphicMargin;

        // coordinates
        juce::Point<float> topLeft { x, y };
        juce::Point<float> botLeft { x, y + graphicSize };
        juce::Point<float> middleRight { x + graphicSize, y + graphicSize/2 };
        juce::Point<float> topRight { x + graphicSize, y };
        juce::Point<float> middleLeft { x, y + graphicSize/2 };
        juce::Point<float> botRight { x + graphicSize, y + graphicSize };

        // drawing
        juce::Path arrowPath;
        if (isLeftArrow){
            arrowPath.startNewSubPath(topLeft);
            arrowPath.lineTo(botLeft);
            arrowPath.lineTo(middleRight);
            arrowPath.closeSubPath();
        } else {
            arrowPath.startNewSubPath(topRight);
            arrowPath.lineTo(botRight);
            arrowPath.lineTo(middleLeft);
            arrowPath.closeSubPath();
        }

        arrowPath = arrowPath.createPathWithRoundedCorners(1.0f);
        g.setColour(Colours::InterfaceMain::textColor);
        g.fillPath(arrowPath);
    }
    
    
private:
    int graphicIndex = 0;
};



class ComboBoxGraphics : public juce::LookAndFeel_V4
{
public:
    ComboBoxGraphics()
    {
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
        
    }
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) override
    {
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();

        juce::Path comboBoxPath, trianglePath, buttonPath;
        
        comboBoxPath.addRoundedRectangle(bounds, 8.0f);
        g.setColour(Colours::InterfaceMain::backgroundFill);
        g.fillPath(comboBoxPath);
        g.setColour(Colours::InterfaceMain::backgroundFillAlt);
        g.strokePath(comboBoxPath, juce::PathStrokeType(2.0f));
        
        auto buttonBounds = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH);
        buttonPath.addRoundedRectangle(buttonBounds, 3.0f);
        
        
        g.setFont(12.0f);
        g.setColour(Colours::InterfaceMain::textColor);
        g.drawText(comboBox.getText(), bounds, juce::Justification::centred, false);
    }
    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override
    {
        juce::Path menuPath;
        menuPath.addRectangle(area);
        
        juce::Colour menuColor;
        menuColor = isHighlighted ? Colours::InterfaceMain::backgroundFillAlt : Colours::InterfaceMain::backgroundFill;
        g.setColour(menuColor);
        g.fillPath(menuPath);
        
        g.setColour(Colours::InterfaceMain::textColor);
        g.setFont(12.0f);
        g.drawText(text, area, juce::Justification::centred, false);
    }
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        juce::Path menuPath;
        menuPath.addRectangle(0, 0, width, height);
        g.setColour(Colours::InterfaceMain::backgroundFillAlt);
        g.fillPath(menuPath);
    }
    
    void drawResizableFrame(juce::Graphics& g, int w, int h, const juce::BorderSize<int>& b) override {}
};

