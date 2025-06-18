/*
  ==============================================================================

    DialGraphics.cpp
    Created: 1 Jun 2025 6:05:37pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "DialGraphics.h"
#include "LookAndFeel.h"
#include "Utility.h"

UserInterfaceGraphics::UserInterfaceGraphics(int graphicIndex)
{
    this->graphicIndex = graphicIndex;
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxTextColourId, Colours::Main::textColor);
}

void UserInterfaceGraphics::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = slider.getLocalBounds().toFloat();
    
    float graphicX = bounds.getX();
    float graphicY = bounds.getY();
    float graphicWidth = bounds.getWidth();
    float graphicHeight = bounds.getHeight();
    lineWidth = bounds.getWidth() * 0.015f;
    
    //==============================================================================
    // Hover Color
    bool hover = slider.isMouseOver();
    
    iconGradientColor = hover ? juce::Colour(Colours::Gradient::gradientHover[stepIndex]) : juce::Colour(Colours::Gradient::gradient[stepIndex]);

    iconWhiteColor = hover ? juce::Colour(Colours::Main::iconWhiteHover) : juce::Colour(Colours::Main::iconWhite);

    // Path stroke

    
    // use enums
    if (graphicIndex == 0){
        drawTone(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 1) {
        drawNoiseFreq(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);

    } else if (graphicIndex == 2){
        drawPitchMod(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 3){
        drawFrequency(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 4){
        drawRepeat(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 5){
        drawAlgorithm(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 6){
        // tension
        
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawTensionIcon(g, graphicX, graphicY + graphicWidth - iconSize,
                        iconSize, 0.5f);
        
        drawTensionIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize,
                        iconSize, 0.0f);

    } else if (graphicIndex == 7){
        // inharmoncity
        
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawInharmIcon(g, graphicX + 2, graphicY + graphicWidth - iconSize,
                        iconSize, 0.25f);
        
        drawInharmIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize,
                        iconSize, 0.0f);
        
    } else if (graphicIndex == 8){
        drawPosition(g, graphicX, graphicY, graphicWidth, graphicWidth, sliderPosProportional);
        
    } else if (graphicIndex == 9){
        drawRate(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);

    } else if (graphicIndex == 10){
        // amp
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawAmpIcon(g, graphicX, graphicY + graphicWidth - iconSize, iconSize, 0.5f);
        drawAmpIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize, iconSize, 1.0f);
    } else if (graphicIndex == 11){
        drawSmallRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    }
}

void UserInterfaceGraphics::drawTone(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float centreX = x + width/2;
    float positionOffset = height * 0.25f * (1.0f - position);
    juce::Point<float>outerTopCoords = { centreX, y + height * 0.35f };
    juce::Point<float>outerBotCoords = { centreX, y + height * 0.65f };
    juce::Point<float>innerTopCoords = { centreX, y + height * 0.375f + positionOffset };
    juce::Point<float>innerBotCoords = { centreX, y + height * 0.625f };
    
    float OuterRadius = width * 0.25f;
    float innerRadius = width * 0.205f;

    juce::Path outerBottomLines, outerTopLines, innerLines;

    //==============================================================================
    // outer arc
    outerBottomLines.addCentredArc(outerBotCoords.x, outerBotCoords.y, OuterRadius, OuterRadius/3, 0.0f, 0.0f, 6.28f, true);
    outerBottomLines.startNewSubPath(outerTopCoords.x - OuterRadius, outerTopCoords.y);
    outerBottomLines.lineTo(outerBotCoords.x - OuterRadius, outerBotCoords.y);
    outerBottomLines.startNewSubPath(outerTopCoords.x + OuterRadius, outerTopCoords.y);
    outerBottomLines.lineTo(outerBotCoords.x + OuterRadius, outerBotCoords.y);
    g.setColour(iconLightGreyColour);
    g.strokePath(outerBottomLines, juce::PathStrokeType(lineWidth));

    //==============================================================================
    // inner arc
    innerLines.addCentredArc(innerTopCoords.x, innerTopCoords.y, innerRadius, innerRadius/3, 0.0f, -1.57f, 1.57f, true);
    innerLines.lineTo(innerBotCoords.x + innerRadius, innerBotCoords.y);
    innerLines.addCentredArc(innerBotCoords.x, innerBotCoords.y, innerRadius, innerRadius/3, 0.0f, 1.57, 4.71f, false);
    innerLines.lineTo(innerTopCoords.x - innerRadius, innerTopCoords.y);

    innerLines.closeSubPath();
    g.setColour(iconGradientColor);
    g.fillPath(innerLines);
    
    // outer arc top layer
    outerTopLines.addCentredArc(outerTopCoords.x, outerTopCoords.y, OuterRadius, OuterRadius/3, 0.0f, 0.0f, 6.28f, true);
    g.setColour(iconLightGreyColour);
    g.strokePath(outerTopLines, juce::PathStrokeType(lineWidth));
}

void UserInterfaceGraphics::drawFrequency(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float panelWidth = width * 0.5f;
    float panelHeight = height * 0.125f;

    //coordinates
    juce::Point<float>leftCoords = { x + width * 0.15f, y + height * 0.5f };
    
    float logPosition = Utility::scaleExp(position, 0.0f, 1.0f, 0.65f);
    float leftMargin = panelWidth * 0.3f;
    float rightMargin = panelWidth * 0.1f;
    float panelWidthScaled = panelWidth * 0.6f;
    float sideMargin = Utility::scale(1.0f - position, 0.0f, 1.0f, rightMargin, leftMargin);
    
    juce::Point<float>topCoords = { leftCoords.x + leftMargin + (panelWidthScaled * logPosition), y + height * 0.4f};
    juce::Point<float>topLeftCoords = { topCoords.x - sideMargin, y + height * 0.5f };
    juce::Point<float>topRightCoords = { topCoords.x + sideMargin, y + height * 0.5f };
    juce::Point<float>rightCoords = { leftCoords.x + panelWidth, leftCoords.y };

    
    juce::Path graphicLines;
    float isoMargin = width * 0.2f;

    // left hand
    graphicLines.startNewSubPath(leftCoords.x, leftCoords.y + panelHeight);
    graphicLines.lineTo(topLeftCoords.x, topLeftCoords.y + panelHeight);
    graphicLines.cubicTo(topLeftCoords.x + sideMargin/2, topLeftCoords.y + panelHeight,
                         topLeftCoords.x + sideMargin/2, topCoords.y + panelHeight,
                         topCoords.x, topCoords.y + panelHeight);
    
    graphicLines.lineTo(topCoords.x + isoMargin, topCoords.y - panelHeight);
    graphicLines.cubicTo(topCoords.x + isoMargin - sideMargin/2, topCoords.y - panelHeight,
                         topLeftCoords.x + isoMargin + sideMargin/2, topLeftCoords.y - panelHeight,
                         topLeftCoords.x + isoMargin, topLeftCoords.y - panelHeight);
    graphicLines.lineTo(leftCoords.x + isoMargin, leftCoords.y - panelHeight);

    graphicLines.closeSubPath();
    g.setColour(iconGradientColor);
    g.fillPath(graphicLines);
    g.strokePath(graphicLines, juce::PathStrokeType(lineWidth));

    // right hand
    graphicLines.startNewSubPath(topCoords.x, topCoords.y + panelHeight);
    graphicLines.cubicTo(topCoords.x + sideMargin/2, topCoords.y + panelHeight,
                         topCoords.x + sideMargin/2, topRightCoords.y + panelHeight,
                         topRightCoords.x, topRightCoords.y + panelHeight);
    graphicLines.lineTo(rightCoords.x, rightCoords.y + panelHeight);
    graphicLines.lineTo(rightCoords.x + isoMargin, rightCoords.y - panelHeight);
    graphicLines.lineTo(topRightCoords.x + isoMargin, topRightCoords.y - panelHeight);
    graphicLines.cubicTo(topRightCoords.x + isoMargin - sideMargin/2, topRightCoords.y - panelHeight,
                         topCoords.x + isoMargin + sideMargin/2, topCoords.y - panelHeight,
                         topCoords.x + isoMargin, topCoords.y - panelHeight);

    graphicLines.closeSubPath();
    g.setColour(iconGradientColor);
    g.fillPath(graphicLines);
    g.strokePath(graphicLines, juce::PathStrokeType(lineWidth));
    
}

void UserInterfaceGraphics::drawPitchMod(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    // coordinates
    float margin = width * 0.25f;
    float radius = width * 0.0275f;

    juce::Point<float>leftCoords = { x + margin, y + height/2 };
    juce::Point<float>rightCoords = { x + width - margin, y + height/2 };
    
    float positionScaled = (width * 0.12f) + width * 0.25f * position;
    juce::Point<float>centerCoords = { x + margin + positionScaled, y + height/2 };
    
    float leftCurveAmount = (std::clamp(1.0f - position, 0.5f, 1.0f) - 0.5f) * height * 0.35f;
    float rightCurveAmount = (std::clamp(1.0f - position, 0.0f, 0.5f) - 0.5f) * height * 0.35f;

    juce::Point<float>leftCurveCoords = { leftCoords.x + positionScaled/2 , y + height/2 + leftCurveAmount };
    juce::Point<float>rightCurveCoords = { rightCoords.x - (rightCoords.x - centerCoords.x)/2 , y + height/2 + rightCurveAmount };
    
    // circles
    juce::Path graphicLines;
    graphicLines.addCentredArc(leftCoords.x, leftCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    graphicLines.addCentredArc(rightCoords.x, rightCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    graphicLines.addCentredArc(centerCoords.x, centerCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    
    g.setColour(iconGradientColor);
    g.fillPath(graphicLines);

    // left lines
    graphicLines.startNewSubPath(leftCoords);
    graphicLines.cubicTo(leftCoords.x + (leftCurveCoords.x - leftCoords.x)/2, leftCoords.y,
                         leftCoords.x + (leftCurveCoords.x - leftCoords.x)/2, leftCurveCoords.y,
                         leftCurveCoords.x, leftCurveCoords.y);
    
    graphicLines.cubicTo(leftCurveCoords.x + (centerCoords.x - leftCurveCoords.x)/2, leftCurveCoords.y,
                         leftCurveCoords.x + (centerCoords.x - leftCurveCoords.x)/2, centerCoords.y,
                         centerCoords.x, centerCoords.y);
    
    // right lines
    graphicLines.startNewSubPath(centerCoords);
    graphicLines.cubicTo(centerCoords.x + (rightCurveCoords.x - centerCoords.x)/2, centerCoords.y,
                         centerCoords.x + (rightCurveCoords.x - centerCoords.x)/2, rightCurveCoords.y,
                         rightCurveCoords.x, rightCurveCoords.y);
    
    graphicLines.cubicTo(rightCurveCoords.x + (rightCurveCoords.x - centerCoords.x)/2, rightCurveCoords.y,
                         rightCurveCoords.x + (rightCurveCoords.x - centerCoords.x)/2, rightCoords.y,
                         rightCoords.x, rightCoords.y);
    
    g.strokePath(graphicLines, juce::PathStrokeType(lineWidth));
}

void UserInterfaceGraphics::drawRepeat(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    // coordinates
    int positionScaled = std::floor(position * 3.0f + 1);
    float widthMargin = width * 0.2;
    width = width * 0.6;
    float segmentSize = width/positionScaled;

    float heightMargin = height * 0.4f;
    height = height * 0.2f;
    float blockMargin = segmentSize * 0.1f;
    float blockSize = segmentSize * 0.8f;
    //==============================================================================
    for (int i = 0; i < positionScaled; i++)
    {
        juce::Path graphicPath;
        float xPosition = widthMargin + blockMargin + i * (blockSize + blockMargin * 2);
        graphicPath.addRoundedRectangle(xPosition, heightMargin, blockSize, height, 3);
        graphicPath = graphicPath.createPathWithRoundedCorners(2);
        
        float alpha = (1.0f/(i + 1)) * 0.5f + 0.5f;
        g.setColour(iconGradientColor.withAlpha(alpha));
        g.fillPath(graphicPath);
    }
}

void UserInterfaceGraphics::drawAlgorithm(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float graphicWidth = width * 0.6f;
    float graphicHeight = height * 0.9f;
    float widthMargin = width * 0.15;
    float heightMargin = width * 0.05;
    float blockSize = (graphicHeight/4) * 0.7f;
    float blockMargin = (graphicHeight/4) * 0.15f;
    
    float algorithmIndex = position * 9.0f;
    algorithmIndex = std::floor(algorithmIndex);

    //==============================================================================
    // simple struct
    enum blockConnect { NONE, DOWN, DOWNLEFT, DOWNRIGHT, LEFTDOWN, RIGHTDOWN };
    struct blockValues
    {
        std::array<int, 4> blockToUse;
        std::array<int, 4> connectValue;
        std::array<juce::String, 4> label = { "3","2", "1", "N" };
    };
    
    blockValues block;
    switch((int)algorithmIndex){
        case 0:
            block.blockToUse = { 4, 7, 10, 11 };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            block.label = { "3","2", "1", "N" };
            break;
            
        case 1:
            block.blockToUse = { 6, 9, 10, 11 };
            block.connectValue = { RIGHTDOWN, DOWN, DOWN, DOWN };
            block.label = { "3","1", "2", "N" };
            break;
            
        case 2:
            block.blockToUse = { 7, 8, 9, 10 };
            block.connectValue = { DOWN, DOWNLEFT, DOWN, DOWN };
            block.label = { "3","2", "N", "1" };

            break;
            
        case 3:
            block.blockToUse = { 4, 6, 7, 10 };
            block.connectValue = { DOWN, DOWNRIGHT, DOWN, DOWN };
            block.label = { "3","N", "2", "1" };
            break;

        case 4:
            block.blockToUse = { 6, 7, 10, 11 };
            block.connectValue = { DOWNRIGHT, RIGHTDOWN, DOWN, DOWN };
            block.label = { "N","3", "1", "2" };
            break;
            
        case 5:
            block.blockToUse = { 6, 7, 8, 10 };
            block.connectValue = { DOWNRIGHT, DOWN, DOWNLEFT, DOWN };
            block.label = { "N","3", "2", "1" };
            break;

        case 6:
            block.blockToUse = { 3, 5, 7, 10 };
            block.connectValue = { DOWNRIGHT, DOWNLEFT, DOWN, DOWN };
            block.label = { "N","3", "2", "1" };
            break;

        case 7:
            block.blockToUse = { 4, 7, 10, 11 };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            block.label = { "N","3", "1", "2" };
            break;

        case 8:
            block.blockToUse = { 4, 7, 8, 10 };
            block.connectValue = { DOWN, DOWN, DOWNLEFT, DOWN };
            block.label = { "N","3", "2", "1" };
            break;
            
        case 9:
            block.blockToUse = { 1, 4, 7, 10 };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            block.label = { "N","3", "2", "1" };
            break;
    }
    
    for (int i = 0; i < 12; i++){ // column
        float xIncr = x + widthMargin + (graphicWidth/3) * (i % 3);
        float yIncr = y + heightMargin + (graphicHeight/4) * (i / 3);
        
        juce::Path blockPath;
        blockPath.addRoundedRectangle(xIncr, yIncr, blockSize, blockSize, 2);
        
        for (int j = 0; j < 4; j++) {
            if (i != block.blockToUse[j]) {
                g.setColour(juce::Colour(iconDarkGreyColour));
                g.strokePath(blockPath, juce::PathStrokeType(lineWidth));

            } else {
                g.setColour(iconWhiteColor);
                g.fillPath(blockPath);
                g.strokePath(blockPath, juce::PathStrokeType(lineWidth));
                break;
            }

        }
    }
    
    for (int i = 0; i < 12; i++){ // column
        float xIncr = x + widthMargin + (graphicWidth/3) * (i % 3);
        float yIncr = y + heightMargin + (graphicHeight/4) * (i / 3);
        
        juce::Path linePath;
        for (int j = 0; j < 4; j++) {
            if (i == block.blockToUse[j]){
                if (block.connectValue[j] == DOWNLEFT){
                    linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                    linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.5f);
                    linePath.lineTo(xIncr - (blockSize + blockMargin), yIncr + (blockSize + blockMargin) * 1.5f);
                    
                } else if (block.connectValue[j] == DOWNRIGHT){
                    linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                    linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.5f);
                    linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                    
                } else if (block.connectValue[j] == DOWN) {
                    linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                    linePath.lineTo(xIncr + blockSize/2, yIncr + (blockSize + blockMargin) * 1.25f);
                    
                } else if (block.connectValue[j] == LEFTDOWN) {
                    linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                    linePath.lineTo(xIncr - (blockSize + blockMargin) * 1.5f, yIncr + blockSize/2);
                    linePath.lineTo(xIncr - (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                    
                } else if (block.connectValue[j] == RIGHTDOWN) {
                    linePath.startNewSubPath(xIncr + blockSize/2, yIncr + blockSize/2);
                    linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + blockSize/2);
                    linePath.lineTo(xIncr + (blockSize + blockMargin) * 1.5f, yIncr + (blockSize + blockMargin) * 1.5f);
                    
                } else if (block.connectValue[j] == NONE) {
                    
                }
            }
        }
        
        linePath = linePath.createPathWithRoundedCorners(3);
        g.setColour(iconWhiteColor);
        
        juce::PathStrokeType strokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(linePath, juce::PathStrokeType(strokeType));
        
    }
        for (int i = 0; i < 12; i++){ // column
            float xIncr = x + widthMargin + (graphicWidth/3) * (i % 3);
            float yIncr = y + heightMargin + (graphicHeight/4) * (i / 3);
            g.setColour(iconDarkGreyColour);
            g.setFont(11.0f);
            
            for (int j = 0; j < 4; j++) {
                if (i == block.blockToUse[j]) {
                        g.drawText(block.label[j], xIncr + 0.75f, yIncr + 0.5f, blockSize, blockSize,juce::Justification::centred);
                }
            }
        }
    
}

void UserInterfaceGraphics::drawRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    //==============================================================================

    float pi = juce::MathConstants<float>::pi;
    float dialStart = 1.25f * pi;
    float dialEnd = 2.75f * pi;
    float sliderPositionScaled = 2.0f + (1.0f - position);
    float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
    

    juce::Path dialBodyPath, dialDotPath, dialOutlinePath, dialSelectPath, tensionLeftPath, tensionRightPath;
    
    float dialOutlineRadius = (width * 0.8f)/2;
    float dialBodyRadius = (width * 0.7f)/2;
    float dialDotRadius = (width * 0.5f)/2;

    dialOutlinePath.addCentredArc(x + width/2, x + width/2,
                                  dialOutlineRadius, dialOutlineRadius,
                                  0.0f, dialStart, dialEnd, true);
    g.setColour(Colours::Main::iconWhite); //

    juce::PathStrokeType strokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(dialOutlinePath, juce::PathStrokeType(strokeType));

    //==============================================================================
    // dial body
    
    dialBodyPath.addCentredArc(x + width/2, y + width/2,
                               dialBodyRadius, dialBodyRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(iconWhiteColor); //
    g.fillPath(dialBodyPath);
    
    //==============================================================================
    // dial dot
    
    juce::Point<float> outlineCoords = {x + width/2 + std::sin(dialPositionInRadians) * dialDotRadius,
        x + width/2 + std::cos(dialPositionInRadians) * dialDotRadius};
    
    juce::Point<float> startCoords = {x + width/2 + std::sin(dialStart) * dialDotRadius,
        x + width/2 + std::cos(dialStart) * dialDotRadius};


    dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                              1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
    
    g.setColour(iconBlackColor);
    g.fillPath(dialDotPath);
    
}

void UserInterfaceGraphics::drawSmallRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float pi = juce::MathConstants<float>::pi;
    float dialStart = 1.25f * pi;
    float dialEnd = 2.75f * pi;
    float sliderPositionScaled = 2.0f + (1.0f - position);
    float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
    
    juce::Path dialBodyPath, dialDotPath, dialOutlinePath;
    float dialOutlineRadius = (width * 0.9f)/2;
    float dialBodyRadius = (width * 0.7f)/2;
    float dialDotRadius = (width * 0.45f)/2;

    dialOutlinePath.addCentredArc(x + width/2, y + width/2,
                                  dialOutlineRadius, dialOutlineRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(juce::Colour(iconDarkGreyColour));
    g.strokePath(dialOutlinePath, juce::PathStrokeType(lineWidth * 1.5f));
    
    //==============================================================================
    // dial body
    dialBodyPath.addCentredArc(x + width/2, y + width/2,
                               dialBodyRadius, dialBodyRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(iconWhiteColor);
    g.fillPath(dialBodyPath);

    //==============================================================================
    // dial dot
    juce::Point<float> outlineCoords = {x + width/2 + std::sin(dialPositionInRadians) * dialDotRadius,
        x + width/2 + std::cos(dialPositionInRadians) * dialDotRadius};
    juce::Point<float> startCoords = {x + width/2 + std::sin(dialStart) * dialDotRadius,
        x + width/2 + std::cos(dialStart) * dialDotRadius};

    dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                              1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
    
    g.setColour(iconBlackColor);
    g.fillPath(dialDotPath);
}

void UserInterfaceGraphics::drawTensionIcon(juce::Graphics& g, float x, float y, float size, float length)
{
    juce::Path graphicPath;
    // start top left, length is 0~1
    graphicPath.addTriangle(x + size * length, y + size * 0.25f,
                            x + size, y + size * 0.75f,
                            x + size * length, y + size * 0.75f);
    graphicPath = graphicPath.createPathWithRoundedCorners(1);
    g.setColour(iconWhiteColor);
    g.fillPath(graphicPath);
}

void UserInterfaceGraphics::drawInharmIcon(juce::Graphics& g, float x, float y, float size, float offset)
{
    juce::Path graphicPath;
    // start top left, length is 0~1
    graphicPath.addEllipse(x, y + size * 0.25f, size * 0.5f, size * 0.5f);
    graphicPath.addEllipse(x + size * offset, y + size * 0.25f,
                           size * 0.5f, size * 0.5f);
    g.setColour(iconWhiteColor);
    g.strokePath(graphicPath, juce::PathStrokeType(lineWidth));
}

void UserInterfaceGraphics::drawPosition(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    juce::Point<float>topLeftCoords = { x + width * 0.3f, y + height * 0.1f };
    juce::Point<float>botLeftCoords = { x + width * 0.3f, y + height * 0.9f };
    juce::Point<float>topRightCoords = { x + width * 0.35f, y + height * 0.1f };
    juce::Point<float>botRightCoords = { x + width * 0.35f, y + height * 0.9f };
    
    juce::Point<float>midCoords = { x + (width * 0.35f + (width * 0.35f)),
        y + (height * 0.2f) + (height * 0.6f * (1.0f - position)) };
    
    juce::Point<float>topMidCoords = { x + width * 0.35f, juce::jlimit(y + height * 0.1f, y + height * 0.9f, midCoords.y - height * 0.15f)};
    
    juce::Point<float>botMidCoords = { x + width * 0.35f, juce::jlimit(y + height * 0.1f, y + height * 0.9f, midCoords.y + height * 0.1f)};


    juce::Path graphicLines;
    graphicLines.startNewSubPath(topRightCoords);
    
    graphicLines.lineTo(topMidCoords);
    graphicLines.lineTo(midCoords);
    graphicLines.lineTo(botMidCoords);

    graphicLines.lineTo(botRightCoords);
    graphicLines = graphicLines.createPathWithRoundedCorners(4.0f);
    
    graphicLines.startNewSubPath(topLeftCoords);
    graphicLines.lineTo(botLeftCoords);
    
    g.setColour(iconWhiteColor);
    juce::PathStrokeType thickStroke(lineWidth * 1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(graphicLines, juce::PathStrokeType(thickStroke));
}

void UserInterfaceGraphics::drawNoiseFreq(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    juce::Path linePath, arcPath, posArcPath;
    juce::Point<float>topCoords = { x + width/2, y + width * 0.15f };
    juce::Point<float>botCoords = { x + width/2, y + width * 0.85f };

    linePath.startNewSubPath(topCoords);
    linePath.lineTo(botCoords);
    g.setColour(iconLightGreyColour);
    g.strokePath(linePath, juce::PathStrokeType(lineWidth * 2.0f));
    
    float radius = width * 0.05f;
    arcPath.addCentredArc(topCoords.x, topCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    arcPath.addCentredArc(botCoords.x, botCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    g.fillPath(arcPath);
    
    float posRadius = width * 0.075f;
    float pos = width * 0.7f * (1.0f - position);
    posArcPath.addCentredArc(x + width/2, (width * 0.15f) + pos , posRadius, posRadius, 0.0f, 0.0f, 6.28f, true);
    g.setColour(iconWhiteColor);
    g.fillPath(posArcPath);

}

void UserInterfaceGraphics::drawRate(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    int positionScaled = std::floor(position * 7.0f);
    float segmentSize = width/7;
    
    float blockMargin = segmentSize * 0.1f;
    float blockSize = segmentSize * 0.8f;
    //==============================================================================
    for (int i = 0; i < 7; i++)
    {
        juce::Path graphicPath;
        float xPosition = i * (blockSize + blockMargin * 2);
        graphicPath.addRoundedRectangle(xPosition, y, blockSize, height, 3);
        graphicPath = graphicPath.createPathWithRoundedCorners(2);
        
        if (i <= positionScaled)
        {
            g.setColour(iconWhiteColor);

        } else {
            g.setColour(iconDarkGreyColour);
        }
        g.fillPath(graphicPath);
    }
}

void UserInterfaceGraphics::drawRateIcon(juce::Graphics& g, float x, float y, float size, int radius)
{
    juce::Path graphicPath;
    // start top left, length is 0~1
    graphicPath.addCentredArc(x + size/2, y + size/2, (size/2) * radius, (size/2) * radius, 0.0f, 0.0f, 6.28f);
    g.setColour(iconWhiteColor);
    g.fillPath(graphicPath);
}

void UserInterfaceGraphics::drawAmpIcon(juce::Graphics& g, float x, float y, float size, float radius)
{
    juce::Path graphicPath, graphicLinePath;
    // start top left, length is 0~1
    graphicPath.addCentredArc(x + size/2, y + size/2, (size/4) * radius, (size/4) * radius, 0.0f, 0.0f, 6.28f);
    g.setColour(iconWhiteColor);
    g.fillPath(graphicPath);
    
    graphicLinePath.addCentredArc(x + size/2, y + size/2, (size/4), (size/4), 0.0f, 0.0f, 6.28f, true);
    g.strokePath(graphicLinePath, juce::PathStrokeType(lineWidth));

}




void UserInterfaceGraphics::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor)
{
    g.setColour(juce::Colour(100, 99, 102));
    juce::Path bgPath;
    bgPath.addRoundedRectangle(textEditor.getLocalBounds().reduced(4.0f, 0.0f), 3.0f);
    g.fillPath(bgPath);
}


juce::Label* UserInterfaceGraphics::createSliderTextBox(juce::Slider& slider)
{
    auto label = LookAndFeel_V4::createSliderTextBox(slider);
    
    label->setFont(juce::Font(12.0f, juce::Font::plain));
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::textColourId, Colours::Main::textColor);

    label->onEditorShow = [label]()
    {
        if(auto* editor = label->getCurrentTextEditor())
        {
            editor->setJustification(juce::Justification::centred);
        }
    };
    return label;
}

