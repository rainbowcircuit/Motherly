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
    setColour(juce::Slider::textBoxTextColourId, Colours::InterfaceMain::textColor);
}

void UserInterfaceGraphics::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = slider.getLocalBounds().toFloat();
    
    float graphicX = bounds.getX();
    float graphicY = bounds.getY();
    float graphicWidth = bounds.getWidth();
    float graphicHeight = bounds.getHeight();
    lineWidth = bounds.getWidth() * 0.015f;
    
    if (graphicIndex == 0){
        drawTone(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 1) {
       // drawPosition(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 2){
        drawPitchMod(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 3){
        drawFrequency(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 4){
        drawProbability(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 5){
        drawAlgorithm(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 6){
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawTensionIcon(g, graphicX, graphicY + graphicWidth - iconSize,
                        iconSize, 0.5f);
        
        drawTensionIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize,
                        iconSize, 0.0f);

    } else if (graphicIndex == 7){
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawInharmIcon(g, graphicX + 2, graphicY + graphicWidth - iconSize,
                        iconSize, 0.25f);
        
        drawInharmIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize,
                        iconSize, 0.0f);
        
    } else if (graphicIndex == 8){
        drawPosition(g, graphicX, graphicY, graphicWidth, graphicWidth, sliderPosProportional);
        
    } else if (graphicIndex == 9){
        drawStepCount(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
    } else if (graphicIndex == 10){
        drawRoundDial(g, graphicX, graphicY, graphicWidth, graphicHeight, sliderPosProportional);
        
        float iconSize = graphicWidth * 0.2f;
        drawRateIcon(g, graphicX, graphicY + graphicWidth - iconSize, iconSize, 1);
        drawRateIcon(g, graphicX + graphicWidth - iconSize, graphicY + graphicWidth - iconSize, iconSize, 4);
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
    // outer arc back layer
    outerBottomLines.addCentredArc(outerBotCoords.x, outerBotCoords.y, OuterRadius, OuterRadius/3, 0.0f, 0.0f, 6.28f, true);
    outerBottomLines.startNewSubPath(outerTopCoords.x - OuterRadius, outerTopCoords.y);
    outerBottomLines.lineTo(outerBotCoords.x - OuterRadius, outerBotCoords.y);
    outerBottomLines.startNewSubPath(outerTopCoords.x + OuterRadius, outerTopCoords.y);
    outerBottomLines.lineTo(outerBotCoords.x + OuterRadius, outerBotCoords.y);
    g.setColour(Colours::StepColour::iconShadow);
    g.strokePath(outerBottomLines, juce::PathStrokeType(lineWidth));

    // inner arc
    innerLines.addCentredArc(innerTopCoords.x, innerTopCoords.y, innerRadius, innerRadius/3, 0.0f, -1.57f, 1.57f, true);
    innerLines.lineTo(innerBotCoords.x + innerRadius, innerBotCoords.y);
    innerLines.addCentredArc(innerBotCoords.x, innerBotCoords.y, innerRadius, innerRadius/3, 0.0f, 1.57, 4.71f, false);
    innerLines.lineTo(innerTopCoords.x - innerRadius, innerTopCoords.y);


    innerLines.closeSubPath();
    g.setColour(Colours::StepColour::stepMainColour[stepIndex]);
    g.fillPath(innerLines);
    
    // outer arc top layer
    outerTopLines.addCentredArc(outerTopCoords.x, outerTopCoords.y, OuterRadius, OuterRadius/3, 0.0f, 0.0f, 6.28f, true);
    g.setColour(Colours::StepColour::iconShadow);
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
    g.setColour(Colours::StepColour::stepMainColour[stepIndex]);
    g.fillPath(graphicLines);

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
    g.setColour(Colours::StepColour::stepMainColourAlt[stepIndex]);
    g.fillPath(graphicLines);
    
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
    
    g.setColour(Colours::StepColour::stepMainColour[stepIndex]);
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

void UserInterfaceGraphics::drawProbability(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    // coordinates
    float radius = width * 0.175f;
    float segmentSize = 6.28f/6;
    float gap = 0.05f;
    float centerGap = 0.5f;
    int positionScaled = std::floor(position * 6.0f);
    for (int i = 0; i < 6; i++)
    {
        float startAngle = segmentSize * i + gap;
        float endAngle = segmentSize * (i + 1) - gap;
        float midAngle = startAngle + endAngle/2;
        juce::Point<float> centerOffset = { (x + width/2) + std::sin(midAngle) * centerGap ,
            (x + width/2) + std::cos(midAngle) * centerGap };
        
        juce::Path graphicPath;
        graphicPath.startNewSubPath(centerOffset);
        graphicPath.addCentredArc(centerOffset.x, centerOffset.y, radius, radius, 0.0f, startAngle, endAngle);
        graphicPath.lineTo(centerOffset);
        graphicPath = graphicPath.createPathWithRoundedCorners(2);
        
        if (i < positionScaled){
            g.setColour(Colours::StepColour::stepMainColour2[stepIndex]);
        } else {
            g.setColour(Colours::StepColour::iconShadow);
        }
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


    enum blockFill { FILL, OPER, NOISE};
    enum blockConnect { NONE, DOWN, DOWNLEFT, DOWNRIGHT, LEFTDOWN, RIGHTDOWN };
    struct blockValues
    {
        std::array<int, 4> blockToUse;
        std::array<int, 4> fillValue;
        std::array<int, 4> connectValue;
    };
    
    blockValues block;
    switch((int)algorithmIndex){
        case 0:
            block.blockToUse = { 4, 7, 10, 11 };
            block.fillValue = { OPER, OPER, OPER, NOISE };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            break;
            
        case 1:
            block.blockToUse = { 6, 9, 10, 11 };
            block.fillValue = { OPER, OPER, OPER, NOISE };
            block.connectValue = { RIGHTDOWN, DOWN, DOWN, DOWN };
            break;
            
        case 2:
            block.blockToUse = { 7, 8, 9, 10 };
            block.fillValue = { OPER, OPER, OPER, NOISE };
            block.connectValue = { DOWN, DOWNLEFT, DOWN, DOWN };
            break;
            
        case 3:
            block.blockToUse = { 4, 6, 7, 10 };
            block.fillValue = { OPER, NOISE, OPER, OPER };
            block.connectValue = { DOWN, DOWNRIGHT, DOWN, DOWN };
            break;

        case 4:
            block.blockToUse = { 6, 7, 10, 11 };
            block.fillValue = { OPER, NOISE, OPER, OPER };
            block.connectValue = { DOWNRIGHT, RIGHTDOWN, DOWN, DOWN };
            break;
            
        case 5:
            block.blockToUse = { 6, 7, 8, 10 };
            block.fillValue = { NOISE, OPER, OPER, OPER };
            block.connectValue = { DOWNRIGHT, DOWN, DOWNLEFT, DOWN };
            break;

        case 6:
            block.blockToUse = { 3, 5, 7, 10 };
            block.fillValue = { OPER, NOISE, OPER, OPER };
            block.connectValue = { DOWNRIGHT, DOWNLEFT, DOWN, DOWN };
            break;

        case 7:
            block.blockToUse = { 4, 7, 10, 11 };
            block.fillValue = { NOISE, OPER, OPER, OPER };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            break;

        case 8:
            block.blockToUse = { 4, 7, 8, 10 };
            block.fillValue = { NOISE, OPER, OPER, OPER };
            block.connectValue = { DOWN, DOWN, DOWNLEFT, DOWN };
            break;
            
        case 9:
            block.blockToUse = { 1, 4, 7, 10 };
            block.fillValue = { NOISE, OPER, OPER, OPER };
            block.connectValue = { DOWN, DOWN, DOWN, DOWN };
            break;
    }
    
    for (int i = 0; i < 12; i++){ // column
        float xIncr = x + widthMargin + (graphicWidth/3) * (i % 3);
        float yIncr = y + heightMargin + (graphicHeight/4) * (i / 3);
        
        juce::Path blockPath;
        blockPath.addRoundedRectangle(xIncr, yIncr, blockSize, blockSize, 2);
        juce::Colour fillColour = Colours::StepColour::stepMainColour[i % 4];
        
        for (int j = 0; j < 4; j++) {
            if (i != block.blockToUse[j]) {
                g.setColour(Colours::StepColour::stepMainColour[i % 4]);
                g.strokePath(blockPath, juce::PathStrokeType(lineWidth));

            } else {
                if (block.fillValue[j] == OPER) {
                    
                    g.setColour(Colours::StepColour::iconWhite);
                    g.fillPath(blockPath);
                    g.strokePath(blockPath, juce::PathStrokeType(lineWidth));

                } else if (block.fillValue[j] == NOISE) {
                    g.setColour(Colours::StepColour::iconWhite);
                    g.fillPath(blockPath);
                    g.strokePath(blockPath, juce::PathStrokeType(lineWidth));

                }
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
        g.setColour(Colours::StepColour::iconWhite);
        g.strokePath(linePath, juce::PathStrokeType(lineWidth));
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
    g.setColour(Colours::StepColour::iconWhite); //

    g.strokePath(dialOutlinePath, juce::PathStrokeType(lineWidth));

    //==============================================================================
    // dial body
    dialBodyPath.addCentredArc(x + width/2, y + width/2,
                               dialBodyRadius, dialBodyRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(Colours::StepColour::iconWhite); //
    g.fillPath(dialBodyPath);
    
    //==============================================================================
    // dial dot
    
    juce::Point<float> outlineCoords = {x + width/2 + std::sin(dialPositionInRadians) * dialDotRadius,
        x + width/2 + std::cos(dialPositionInRadians) * dialDotRadius};
    
    juce::Point<float> startCoords = {x + width/2 + std::sin(dialStart) * dialDotRadius,
        x + width/2 + std::cos(dialStart) * dialDotRadius};


    dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                              1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
    
    g.setColour(Colours::StepColour::iconShadow);
    g.fillPath(dialDotPath);
    
}

void UserInterfaceGraphics::drawSmallRoundDial(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float pi = juce::MathConstants<float>::pi;
    float dialStart = 1.25f * pi;
    float dialEnd = 2.75f * pi;
    float sliderPositionScaled = 2.0f + (1.0f - position);
    float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
    

    juce::Path dialBodyPath, dialDotPath;
    float dialBodyRadius = (width * 0.7f)/2;
    float dialDotRadius = (width * 0.5f)/2;

    // dial body
    dialBodyPath.addCentredArc(x + width/2, y + width/2,
                               dialBodyRadius, dialBodyRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(Colours::StepColour::iconWhite);
    g.strokePath(dialBodyPath, juce::PathStrokeType(lineWidth * 2.5f));

    //==============================================================================
    // dial dot
    juce::Point<float> outlineCoords = {x + width/2 + std::sin(dialPositionInRadians) * dialDotRadius,
        x + width/2 + std::cos(dialPositionInRadians) * dialDotRadius};
    juce::Point<float> startCoords = {x + width/2 + std::sin(dialStart) * dialDotRadius,
        x + width/2 + std::cos(dialStart) * dialDotRadius};

    dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                              1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
    
    g.setColour(Colours::StepColour::iconWhite);
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
    g.setColour(Colours::StepColour::iconWhite);
    g.fillPath(graphicPath);
}

void UserInterfaceGraphics::drawInharmIcon(juce::Graphics& g, float x, float y, float size, float offset)
{
    juce::Path graphicPath;
    // start top left, length is 0~1
    graphicPath.addEllipse(x, y + size * 0.25f, size * 0.5f, size * 0.5f);
    graphicPath.addEllipse(x + size * offset, y + size * 0.25f,
                           size * 0.5f, size * 0.5f);
    g.setColour(Colours::StepColour::iconWhite);
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


    juce::Path graphicLines, arrowLines;
    graphicLines.startNewSubPath(topRightCoords);
    
    graphicLines.lineTo(topMidCoords);
    graphicLines.lineTo(midCoords);
    graphicLines.lineTo(botMidCoords);

    graphicLines.lineTo(botRightCoords);
    graphicLines = graphicLines.createPathWithRoundedCorners(4.0f);
    
    graphicLines.startNewSubPath(topLeftCoords);
    graphicLines.lineTo(botLeftCoords);
    g.setColour(Colours::StepColour::iconWhite);
    g.strokePath(graphicLines, juce::PathStrokeType(lineWidth * 1.5f));
}

void UserInterfaceGraphics::drawRateIcon(juce::Graphics& g, float x, float y, float size, int div)
{
    juce::Path graphicPath;

    for (int i = 0; i < div; i++)
    {
        float blockWidth = (size * 0.8f)/div;
        graphicPath.addRoundedRectangle(x + (size/div) * i, y + size * 0.25f,
                                        blockWidth, size * 0.5f, 2);
        g.setColour(Colours::StepColour::iconWhite);
        g.fillPath(graphicPath);
    }
}



void UserInterfaceGraphics::drawStepCount(juce::Graphics& g, float x, float y, float width, float height, float position)
{
    float widthMargin = width * 0.025f;
    width = width * 0.95f;
    
    float index = position * 8.0f;
    index = std::floor(index);
    float blockSize = (width/4) * 0.6f;
    float blockMargin = (width/4) * 0.2f;

    for (int i = 0; i < 8; i++)
    {
        juce::Path graphicPath;
        
        int j = i % 4;
        int k = i / 4;
        graphicPath.addRoundedRectangle(widthMargin + (blockMargin + blockSize) * j,
                                        blockSize + ((blockMargin + blockSize) * k),
                                        blockSize, blockSize, 2);
        
        g.setColour(Colours::StepColour::iconWhite);
        
        if (i > index){
            g.strokePath(graphicPath, juce::PathStrokeType(lineWidth));
        } else {
            g.fillPath(graphicPath);
        }
    }
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
    label->setColour(juce::Label::textColourId, Colours::InterfaceMain::textColor);

    label->onEditorShow = [label]()
    {
        if(auto* editor = label->getCurrentTextEditor())
        {
            editor->setJustification(juce::Justification::centred);
        }
    };
    return label;
}

