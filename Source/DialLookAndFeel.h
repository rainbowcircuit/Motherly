#pragma once
#include <JuceHeader.h>
#include "ColorLookAndFeel.h"
class UserInterfaceGraphics : public juce::LookAndFeel_V4
{
public:
    UserInterfaceGraphics(int graphicIndex);
    ~UserInterfaceGraphics() {}

    void setStepIndex(int stepIndex);
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
    //==============================================================================
    
    void drawRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawSmallRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawPitch(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawTone(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawPitchEnv(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawRepeat(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawTensionIcon(juce::Graphics& g, float x, float y, float size, float length);
    void drawInharmIcon(juce::Graphics& g, float x, float y, float size, float offset);
    void drawPosition(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawNoiseFreq(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawAlgorithm(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawRate(juce::Graphics& g, float x, float y, float width, float height, float position);
    void drawRateIcon(juce::Graphics& g, float x, float y, float size, int div);
    void drawAmpIcon(juce::Graphics& g, float x, float y, float size, float radius);

    //==============================================================================
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& texteditor) override {}
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override;
    juce::Label* createSliderTextBox(juce::Slider& slider) override;

private:
    int graphicIndex = 0, stepIndex = 0;
    float lineWidth = 1.0f;
    
    // colors
    juce::Colour iconWhiteColor;
    juce::Colour iconGradientColor;
    juce::Colour iconBlackColor = juce::Colour(Colors::Main::backgroundFill);
    juce::Colour iconLightGreyColour = juce::Colour(Colors::Main::iconLightGrey);
    juce::Colour iconDarkGreyColour = juce::Colour(Colors::Main::iconDarkGrey);
};
