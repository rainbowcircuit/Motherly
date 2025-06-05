/*
  ==============================================================================

    DialGraphics.h
    Created: 1 Jun 2025 6:05:37pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
class UserInterfaceGraphics : public juce::LookAndFeel_V4
{
public:
    UserInterfaceGraphics(int graphicIndex);
    ~UserInterfaceGraphics() {}

    void setStepIndex(int stepIndex)
    {
        this->stepIndex = stepIndex;
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
    void drawRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawSmallRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position);

    void drawFrequency(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawTone(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawPitchMod(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawProbability(juce::Graphics& g, float x, float y, float width, float height, float position);

    void drawTensionIcon(juce::Graphics& g, float x, float y, float size, float length);

    void drawInharmIcon(juce::Graphics& g, float x, float y, float size, float offset);

    void drawPosition(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawAlgorithm(juce::Graphics& g, float x, float y, float width, float height, float position);
    
    void drawRateIcon(juce::Graphics& g, float x, float y, float size, int div);

    void drawStepCount(juce::Graphics& g, float x, float y, float width, float height, float position);

    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& texteditor) override {}
    
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override;
    
    juce::Label* createSliderTextBox(juce::Slider& slider) override;

private:
    enum dialSelect {
        FREQUENCY,
        TONE,
        MOD,
        PROB,
        TENSION,
        INHARM,
        POSITION,
        ALGORITHM,
    };
    
    
    int graphicIndex = 0, stepIndex;
    float lineWidth;
};


/*
class NoiseGraphics : public juce::Component
{
public:
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds = bounds.reduced(5, 5);

        juce::Path borderPath, linePath, dotPath;

        borderPath.addRoundedRectangle(bounds, bounds.getWidth() * 0.1f);
        g.setColour(Colours::StepColour::iconWhite);
        g.strokePath(borderPath, juce::PathStrokeType(1));
        
        auto innerBounds = bounds.reduced(3, 3);
        borderPath.addRoundedRectangle(innerBounds, bounds.getWidth() * 0.05f);
        g.setColour(Colours::StepColour::iconShadow);
        g.strokePath(borderPath, juce::PathStrokeType(1));

        float dotSize = bounds.getWidth() * 0.075f;
        dotPath.addCentredArc(xPos, yPos, dotSize, dotSize, 0.0f, 0.0f, 6.28f);
        g.setColour(Colours::StepColour::iconWhite);
        g.fillPath(dotPath);
        
    }
    
    void resized() override {}
    
    void mouseDown(const juce::MouseEvent &m) override
    {
        auto position = m.getPosition();
        xPos = position.x;
        yPos = position.y;
        repaint();
    }
    
    void mouseDrag(const juce::MouseEvent &m) override
    {
        auto position = m.getPosition();
        xPos = position.x;
        yPos = position.y;
        repaint();
    }
    
    void mouseUp(const juce::MouseEvent &m) override
    {
        auto position = m.getPosition();
        xPos = position.x;
        yPos = position.y;
        repaint();

        
    }
    
private:
    float xPos = 10, yPos = 5.0f;
};
*/
