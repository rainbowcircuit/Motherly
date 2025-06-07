#include "PatchBayGraphics.h"

PatchBay::PatchBay(MotherlyAudioProcessor& p) : audioProcessor (p)
{    
    for(int point = 0; point < 20; point++)
    {
        patchPoint[point].setIsInput(patchPointLayout[point].isInput);
        patchPoint[point].setIsAvailable(true);
        patchPoint[point].setLabelText(patchPointLayout[point].labelText);
        
        addAndMakeVisible(patchPoint[point]);
        patchPoint[point].setInterceptsMouseClicks(false, false);

        if (!patchPointLayout[point].isInput)
        {
            patchCable[point].setYOffset(40 * 0.15f); // 40 is hard coded
            addAndMakeVisible(patchCable[point]);
            patchCable[point].setInterceptsMouseClicks(false, false);
        }
    }
    
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }
    
    parameterIndexAtomic = 0;
    newValueAtomic = 0.0f;

    resized();
}
 
PatchBay::~PatchBay()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params){
        param->removeListener(this);
    }

}

void PatchBay::paint(juce::Graphics& g)
{
    auto bg = getLocalBounds().toFloat();
    bg.reduce(5, 5);
    
    juce::Path bgFill;
    bgFill.addRoundedRectangle(bg.getX(), bg.getY(), bg.getWidth(), bg.getHeight(), 5);
    g.setColour(Colours::InterfaceMain::backgroundFill);
    g.fillPath(bgFill);
}

void PatchBay::resized()
{
    auto bounds = getLocalBounds().toFloat();

    patchBayLabel.setBounds(bounds.getCentreX(), bounds.getCentreY(), bounds.getWidth(), bounds.getHeight() * 0.1f);
    
    float xIncr = (bounds.getWidth() * 0.9f)/4;
    float yIncr = (bounds.getHeight() * 0.8f)/5;
    for(int point = 0; point < 20; point++)
    {
        int xIndex = point % 4;
        int yIndex = point / 4;

        juce::Point pointCoords = { (bounds.getWidth() * 0.0775f) + (xIndex * xIncr),
            (bounds.getHeight() * 0.175f) + (yIndex * yIncr) };
        
        patchPoint[point].setBounds(pointCoords.x, pointCoords.y, 30, 40);
        
        
        if (!patchPointLayout[point].isInput)
        {
            patchCable[point].setBounds(getLocalBounds());
            patchCable[point].setAlwaysOnTop(true);
        }
    }
}

void PatchBay::mouseDown(const juce::MouseEvent &m)
{
    for(int point = 0; point < 20; point++)
    {
        auto mousePosition = m.getEventRelativeTo(&patchPoint[point]).getPosition();

        if (!patchPointLayout[point].isInput && patchPoint[point].isMouseOver(mousePosition))
        {
            prevCableIndex.reset();
            auto outputPosition = patchPoint[point].getBounds().getCentre().toFloat();

            DBG("Any output, regardless of availability: " << point);
            patchCable[point].setCableConnected(false);
            patchCable[point].setCableInUse(true);
            patchCable[point].setOutputPosition(outputPosition);
            patchCable[point].setMousePosition(m);
            activeCableIndex = point;
            
        } else if (patchPointLayout[point].isInput && patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Available Input: " << point);
            break;
            
        } else if (patchPointLayout[point].isInput && !patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Unavailable Input: " << point);
            
            int outputIndex = patchPoint[point].getConnectionIndex();
            DBG("Output associated with this input: " << outputIndex);

            auto outputPosition = patchPoint[outputIndex].getBounds().getCentre().toFloat();
            
            patchCable[outputIndex].setCableConnected(false);
            patchCable[outputIndex].setCableInUse(true);
            patchCable[outputIndex].setOutputPosition(outputPosition);
            patchCable[outputIndex].setMousePosition(m);
            
            patchPoint[point].setIsAvailable(false);
            activeCableIndex = outputIndex;
            prevCableIndex = point;
        }
    }
}

void PatchBay::mouseUp(const juce::MouseEvent &m)
{
    for(int point = 0; point < 20; point++)
    {
        auto mousePosition = m.getEventRelativeTo(&patchPoint[point]).getPosition();

        if (!activeCableIndex.has_value() && !patchPointLayout[point].isInput && patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Available Output without Cable: " << point);
            break;
            
        } else if (!activeCableIndex.has_value() && !patchPointLayout[point].isInput && !patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Unavailable Output without Cable: " << point);
            break;

        } else if (!activeCableIndex.has_value() && patchPointLayout[point].isInput && patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Available Input without Cable: " << point);
            break;

        } else if (!activeCableIndex.has_value() && patchPointLayout[point].isInput && !patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Unavailable Input without Cable: " << point);
            break;

        } else if (activeCableIndex.has_value() && !patchPointLayout[point].isInput && patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Available Output with Cable: " << point);
            
            patchCable[*activeCableIndex].setCableConnected(false);
            patchCable[*activeCableIndex].setCableInUse(false);
            patchPoint[*activeCableIndex].setIsAvailable(true);
            if (prevCableIndex.has_value())
            {
                patchPoint[*prevCableIndex].setIsAvailable(true);
                prevCableIndex.reset();
            }
            activeCableIndex.reset();
            // parameter clearing
            clearParameterValues(*activeCableIndex);
            break;

        } else if (activeCableIndex.has_value() && !patchPointLayout[point].isInput && !patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Unavailable Output with Cable: " << point);
            
            patchCable[*activeCableIndex].setCableConnected(false);
            patchCable[*activeCableIndex].setCableInUse(false);
            patchPoint[*activeCableIndex].setIsAvailable(true);
            if (prevCableIndex.has_value())
            {
                patchPoint[*prevCableIndex].setIsAvailable(true);
                prevCableIndex.reset();
            }
            activeCableIndex.reset();
            clearParameterValues(*activeCableIndex);

            break;

        } else if (activeCableIndex.has_value() && patchPointLayout[point].isInput && patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Available Input with Cable: " << point);
            
            auto inputPosition = patchPoint[point].getBounds().getCentre().toFloat();

            setParameterValues(*activeCableIndex, point);
            
            patchCable[*activeCableIndex].setCableConnected(true);
            patchCable[*activeCableIndex].setCableInUse(true);
            patchCable[*activeCableIndex].setConnectionIndex(*activeCableIndex, point);
            patchCable[*activeCableIndex].setInputPosition(inputPosition);

            patchPoint[point].setIsAvailable(false);
            patchPoint[*activeCableIndex].setIsAvailable(false);

            patchPoint[point].setConnectionIndex(*activeCableIndex); // mark input with output index
            patchPoint[*activeCableIndex].setConnectionIndex(*activeCableIndex); // mark output with output index

            
            if (prevCableIndex.has_value())
            {
                patchPoint[*prevCableIndex].setIsAvailable(true);
            //    prevCableIndex.reset();
            }
            activeCableIndex.reset();
            prevCableIndex = point;
            break;
            
        } else if (!patchPoint[point].isAvailable() && patchPoint[point].isMouseOver(mousePosition))
        {
            DBG("Unavailable Input or Output with Cable: " << point);
            
            patchCable[*activeCableIndex].setCableConnected(false);
            patchCable[*activeCableIndex].setCableInUse(false);
            patchPoint[*activeCableIndex].setIsAvailable(true);
            if (prevCableIndex.has_value())
            {
                patchPoint[*prevCableIndex].setIsAvailable(true);
                prevCableIndex.reset();
            }
            activeCableIndex.reset();
            patchPoint[point].setMouseOver(false);
            // clear parameters
          //  clearParameterValues(*activeCableIndex);
            break;
            
        }
    }
    
    if (activeCableIndex.has_value())
    {
        DBG("Released mouse not over patch point; clearing active cable.");
        setParameterValues(*activeCableIndex, 0); // 0 is no input
        patchCable[*activeCableIndex].setCableConnected(false);
        patchCable[*activeCableIndex].setCableInUse(false);
        patchPoint[*activeCableIndex].setIsAvailable(true);
        int inputAssociated = patchCable[*activeCableIndex].getConnectionIndex(false);
        patchPoint[inputAssociated].setIsAvailable(true);

        if (prevCableIndex.has_value())
        {
            patchPoint[*prevCableIndex].setIsAvailable(true);
            prevCableIndex.reset();
        }
        activeCableIndex.reset();
    }
}

void PatchBay::mouseDrag(const juce::MouseEvent &mousePosition)
{
    if (activeCableIndex.has_value()){
        patchCable[*activeCableIndex].setMousePosition(mousePosition);
    }
    
    for(int point = 0; point < 20; point++)
    {
        //******Set hover while drag******//
        auto localPos = mousePosition.getEventRelativeTo(&patchPoint[point]).getPosition();

        if (activeCableIndex.has_value() && patchPointLayout[point].isInput && patchPoint[point].isMouseOver(localPos) && patchPoint[point].isAvailable())
        {
            patchPoint[point].setMouseOver(true);
            break;
        } else {
            patchPoint[point].setMouseOver(false);
        }
    }
}

void PatchBay::setParameterValues(int output, int input)
{
    juce::String outputParam = patchPointLayout[output].paramID;
    int inputIndex = patchPointLayout[input].localInputIndex;
    
    auto* param = audioProcessor.apvts.getParameter(outputParam);
    const juce::NormalisableRange<float> range = param->getNormalisableRange();
    float normalised = range.convertTo0to1(inputIndex);
    
    audioProcessor.apvts.getParameter(outputParam)->setValueNotifyingHost(normalised);
    
 //   DBG(outputParam << "with index: " << inputIndex << " normalised: " << normalised);
}

void PatchBay::clearParameterValues(int output)
{
    for(int point = 0; point < 20; point++){
        if (patchPointLayout[point].isInput){
            audioProcessor.apvts.getParameter(patchPointLayout[output].paramID)->setValueNotifyingHost(0);
        }
    }
}

void PatchBay::setCableFromParameter(int output, int input)
{
    int outputIndex = getLocalIndex(output + 1, false); // + 1 since output is 0 indexed
    int inputIndex = getLocalIndex(input, true);
    auto outputPos = patchPoint[outputIndex].getBounds().getCentre().toFloat();
    auto inputPos = patchPoint[inputIndex].getBounds().getCentre().toFloat();

    // no input
    if (inputIndex == 0){
        patchCable[outputIndex].setCableInUse(false);
        patchCable[outputIndex].setCableConnected(false);
        patchPoint[outputIndex].setIsAvailable(true);
        patchPoint[inputIndex].setIsAvailable(true);
        return;
    }
    
    // clear previous input
    int prevInputIndex = patchCable[outputIndex].getConnectionIndex(false);
    if (inputIndex != prevInputIndex)
    {
        patchPoint[prevInputIndex].setIsAvailable(true);
    }
    
    // set cable and patch point
    patchPoint[inputIndex].setIsAvailable(false);
    patchPoint[outputIndex].setIsAvailable(false);
    // have input remember output, and vice versa
    patchPoint[inputIndex].setConnectionIndex(outputIndex);
    patchPoint[outputIndex].setConnectionIndex(inputIndex);
    
    patchCable[outputIndex].setOutputPosition(outputPos);
    patchCable[outputIndex].setInputPosition(inputPos);
    patchCable[outputIndex].setCableInUse(true);
    patchCable[outputIndex].setCableConnected(true);
    patchCable[outputIndex].setConnectionIndex(outputIndex, inputIndex);
    
    juce::String debug = "cable set from param, output: " + juce::String(outputIndex) + " input: " + juce::String(input);
    DBG(debug);
    prevInputIndex = inputIndex;
    
    activeCableIndex.reset();
    // prevCableIndex.reset();
    repaint();
}

int PatchBay::getLocalIndex(int indexToFind, bool findInput)
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

void PatchBay::clearUnusedConnection()
{
}


PatchPoint::PatchPoint()
{
    addAndMakeVisible(patchLabel);
    patchLabel.setFont(10.0f);
    patchLabel.setJustificationType(juce::Justification::centred);
}

void PatchPoint::paint(juce::Graphics &g)
{
    juce::Colour bgFillColour = !isInput ? Colours::StepColour::iconWhite : Colours::InterfaceMain::backgroundHoverFill; // for now
    
    juce::Colour patchColour = { 100, 100, 100 };
    
    if (available){
        patchColour = { 100, 100, 100 };
    } else if (!available){
        patchColour = { 150, 150, 150 };
    }
    auto bounds = getLocalBounds().toFloat();
    
    juce::Path bgPath, graphicsPath;
    bgPath.addRoundedRectangle(bounds, bounds.getWidth() * 0.15f);
    g.setColour(bgFillColour);
    g.fillPath(bgPath);
    
    graphicsPath.addCentredArc(x, y, 2, 2, 0.0f, 0.0f, 6.28f, true);
    g.setColour(patchColour);
    g.fillPath(graphicsPath);
    
    float radius = mouseOver ? 7.0f : 6.0f;
    graphicsPath.addCentredArc(x, y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    g.strokePath(graphicsPath, juce::PathStrokeType(2.0f));
}

void PatchPoint::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float yOffset = getLocalBounds().getHeight() * 0.15f;
    
    x = bounds.getCentreX();
    y = bounds.getCentreY() + yOffset;
    
    patchLabel.setBounds(bounds.getX(), bounds.getCentreY() - bounds.getHeight() * 0.4f, bounds.getWidth(), bounds.getHeight()/3);
}

void PatchPoint::setLabelText(juce::String labelText)
{
    this->labelText = labelText;
    patchLabel.setText(labelText, juce::dontSendNotification);

}

bool PatchPoint::isMouseOver(juce::Point<int> mousePos)
{
    juce::Rectangle<int> hitArea(x - 7, y - 7, 14, 14);
    bool isMouseOver = hitArea.contains(mousePos);
    return isMouseOver;
}

void PatchPoint::setMouseOver(bool mouseOver)
{
    this->mouseOver = mouseOver;
}

void PatchPoint::setConnectionIndex(int index)
{
    connectionIndex = index;
}

int PatchPoint::getConnectionIndex()
{
    return connectionIndex;
}

bool PatchPoint::isAvailable()
{
    return available;
}
   
void PatchPoint::setIsAvailable(bool availablity)
{
    available = availablity;
}

void PatchPoint::setIsInput(bool input)
{
    isInput = input;
    juce::Colour labelTextColour = isInput ? Colours::InterfaceMain::textColor : Colours::InterfaceMain::backgroundFillAlt;
    patchLabel.setColour(juce::Label::textColourId, labelTextColour);
}

PatchCable::PatchCable() {}

void PatchCable::paint(juce::Graphics& g)
{
    juce::Path cablePath, cableEndPath;
    juce::Colour fillColour = Colours::StepColour::stepMainColour[getConnectionIndex(true)];
    g.setColour(fillColour);

    if (cableInUse){
        // draw output arc and path start
        cableEndPath.addCentredArc(outputPoint.x, outputPoint.y, 4, 4, 0.0f, 0.0f, 6.28f, true);
        g.fillPath(cableEndPath);
        cablePath.startNewSubPath(outputPoint);
        
        if (cableConnected)
        {
            // draw to input point and its arc
            cablePath.cubicTo(outputPoint.x, outputPoint.x + 10, inputPoint.x, inputPoint.y + 10, inputPoint.x, inputPoint.y);
            g.strokePath(cablePath, juce::PathStrokeType(4.0));
            cableEndPath.addCentredArc(inputPoint.x, inputPoint.y, 4, 4, 0.0f, 0.0f, 6.28f, true);
            g.fillPath(cableEndPath);
            
        } else {
            // draw to mouse
            cablePath.cubicTo(outputPoint.x, outputPoint.x + 10, mousePoint.x, mousePoint.y + 10, mousePoint.x, mousePoint.y);
            g.strokePath(cablePath, juce::PathStrokeType(4.0));
            cableEndPath.addCentredArc(mousePoint.x, mousePoint.y, 4, 4, 0.0f, 0.0f, 6.28f, true);
            g.fillPath(cableEndPath);
        }
        g.strokePath(cablePath, juce::PathStrokeType(4.0));
    }
}

void PatchCable::setMousePosition(const juce::MouseEvent &m)
{
    mousePoint.x = m.getPosition().getX();
    mousePoint.y = m.getPosition().getY();
    repaint();
}
    
void PatchCable::setOutputPosition(const juce::Point<float> &p)
{
    outputPoint.x = p.x;
    outputPoint.y = p.y + yOffset;
    repaint();
}

void PatchCable::setInputPosition(const juce::Point<float> &p)
{
    inputPoint.x = p.x;
    inputPoint.y = p.y + yOffset;
    repaint();
}

void PatchCable::setCableInUse(bool inUse)
{
    cableInUse = inUse;
}

bool PatchCable::isCableInUse()
{
    return cableInUse;
}

void PatchCable::setCableConnected(bool isConnected)
{
    cableConnected = isConnected;
}

bool PatchCable::isCableConnected()
{
    return cableConnected;
}

void PatchCable::setConnectionIndex(int output, int input)
{
    cableOutput = output;
    cableInput = input;
}

int PatchCable::getConnectionIndex(bool getOutput)
{
    if (getOutput)
    {
        return cableOutput;
    } else {
        return cableInput;
    }
}

void PatchCable::setYOffset(float yOffset)
{
    this->yOffset = yOffset;
}

