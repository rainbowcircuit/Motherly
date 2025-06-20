#include "MiscGraphics.h"

ButtonGraphics::ButtonGraphics(int graphicIndex)
{
    this->graphicIndex = graphicIndex;
}

void ButtonGraphics::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = juce::Rectangle<int>(button.getWidth(), button.getWidth()).toFloat();
    bounds.reduce(5, 5);
    
    juce::Path bgPath;
    bgPath.addRoundedRectangle(bounds, 5.0f);
    juce::Colour buttonColour = shouldDrawButtonAsDown ? Colours::Main::backgroundHoverFill : Colours::Main::backgroundFill;
    g.setColour(buttonColour);
    g.fillPath(bgPath);
    g.setColour(Colours::Main::backgroundFill);
    g.strokePath(bgPath, juce::PathStrokeType(2.0f));
    
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    bool arrowDirection = (graphicIndex == 1);
    if(graphicIndex == 0){
        drawSaveButton(g, x, y, width, height);
        
    } else if (graphicIndex == 1) {
        drawArrowButton(g, x, y, width, height, arrowDirection);
        
    } else if (graphicIndex == 2) {
        drawArrowButton(g, x, y, width, height, arrowDirection);
    }
}
