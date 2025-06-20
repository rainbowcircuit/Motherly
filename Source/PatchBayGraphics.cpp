#include "PatchBayGraphics.h"

PatchPoint::PatchPoint()
{
    addAndMakeVisible(patchLabel);
    patchLabel.setFont(10.0f);
    patchLabel.setJustificationType(juce::Justification::centred);
}

void PatchPoint::paint(juce::Graphics &g)
{
    juce::Colour bgFillColour = !isInput ? Colours::Main::iconWhite : Colours::Main::iconDarkGrey; // for now
    juce::Colour patchColour = !isInput ? Colours::Main::iconBlack : Colours::Main::iconWhite;
    auto bounds = getLocalBounds().toFloat();
    
    juce::Path bgPath, graphicsPath;
    bgPath.addRoundedRectangle(bounds, bounds.getWidth() * 0.15f);
    g.setColour(bgFillColour);
    g.fillPath(bgPath);
    
    graphicsPath.addCentredArc(x, y, 2.75, 2.75, 0.0f, 0.0f, 6.28f, true);
    g.setColour(patchColour);
    g.fillPath(graphicsPath);
    
    float radius = mouseOver ? 7.0f : 6.0f;
    graphicsPath.addCentredArc(x, y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    g.strokePath(graphicsPath, juce::PathStrokeType(1.5f));
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
    juce::Rectangle<int> hitArea(x - 9, y - 9, 18, 18);
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
    juce::Colour labelTextColour = isInput ? Colours::Main::iconWhite : Colours::Main::iconDarkGrey;
    patchLabel.setColour(juce::Label::textColourId, labelTextColour);
}

PatchCable::PatchCable()
{
    juce::Random cableRand;
    cableColor = cableRand.nextInt(6) + 1;
}

void PatchCable::paint(juce::Graphics& g)
{
    juce::Path cablePath, cableEndPath;
    juce::Colour fillColour = Colours::Gradient::gradientDarker[cableColor];
    g.setColour(fillColour);

    if (cableInUse){
        // draw output arc and path start
        cableEndPath.addCentredArc(outputPoint.x, outputPoint.y, 4, 4, 0.0f, 0.0f, 6.28f, true);
        g.fillPath(cableEndPath);
        cablePath.startNewSubPath(outputPoint);
        
        if (cableConnected)
        {
            float dx = inputPoint.x - outputPoint.x;
            float dy = inputPoint.y - outputPoint.y;
            float slack = std::abs(dy) * 0.5f + 20.0f;

            
            juce::Point<float> bezierCoords1(outputPoint.x + dx * 0.15f, outputPoint.y + slack);
            juce::Point<float> bezierCoords2(inputPoint.x - dx * 0.15f, inputPoint.y + 20.0f);

            // draw to input point and its arc
            cablePath.cubicTo(bezierCoords1, bezierCoords2, inputPoint);
            g.strokePath(cablePath, juce::PathStrokeType(4.0));
            cableEndPath.addCentredArc(inputPoint.x, inputPoint.y, 4, 4, 0.0f, 0.0f, 6.28f, true);
            g.fillPath(cableEndPath);
            
        } else {
            float dx = mousePoint.x - outputPoint.x;
            float dy = mousePoint.y - outputPoint.y;
            float slack = std::abs(dy) * 0.5f + 40.0f;
            juce::Point<float> bezierCoords1(outputPoint.x + dx * 0.15f, outputPoint.y + slack);
            juce::Point<float> bezierCoords2(mousePoint.x - dx * 0.15f, mousePoint.y + 40.0f);

            // draw to mouse
            cablePath.cubicTo(bezierCoords1, bezierCoords2, mousePoint);
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
    addAndMakeVisible(patchBayLabel);
    patchBayLabel.setText("Input/Output", juce::dontSendNotification);
    patchBayLabel.setColour(juce::Label::textColourId, Colours::Main::textColor);
    patchBayLabel.setFont(12.0f);
    patchBayLabel.setJustificationType(juce::Justification::centred);

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
    g.setColour(Colours::Main::backgroundFill);
    g.fillPath(bgFill);
}

void PatchBay::resized()
{
    auto bounds = getLocalBounds().toFloat();
    patchBayLabel.setBounds(bounds.getX(), bounds.getY() + bounds.getHeight() * 0.075f, bounds.getWidth(), bounds.getHeight() * 0.05f);
    
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
    for (int point = 0; point < 20; point++)
    {
        auto& patch = patchPoint[point];
        auto mousePosition = m.getEventRelativeTo(&patch).getPosition();

        if (!patch.isMouseOver(mousePosition))
            continue;

        bool isInput = patchPointLayout[point].isInput;
        bool isAvailable = patch.isAvailable();
        bool hasCable = activeCableIndex.has_value();

        if (!hasCable)
        {
            if (!isInput && isAvailable)
                DBG("Available Output without Cable: " << point);
            else if (!isInput && !isAvailable)
                DBG("Unavailable Output without Cable: " << point);
            else if (isInput && isAvailable)
                DBG("Available Input without Cable: " << point);
            else if (isInput && !isAvailable)
                DBG("Unavailable Input without Cable: " << point);
            
            break;
        }
        else
        {
            auto cableIdx = *activeCableIndex;

            if (!isInput && isAvailable)
            {
                DBG("Available Output with Cable: " << point);
                disconnectCable(cableIdx);
                break;
            }
            else if (!isInput && !isAvailable)
            {
                DBG("Unavailable Output with Cable: " << point);
                disconnectCable(cableIdx);
                break;
            }
            else if (isInput && isAvailable)
            {
                DBG("Available Input with Cable: " << point);
                connectCableToInput(cableIdx, point);
                break;
            }
            else
            {
                DBG("Unavailable Input or Output with Cable: " << point);
                disconnectCable(cableIdx);
                patch.setMouseOver(false);
                break;
            }
        }
    }

    if (activeCableIndex.has_value())
    {
        DBG("Released mouse not over patch point; clearing active cable.");
        disconnectCable(*activeCableIndex, true); // optional fallback disconnect
    }
}

void PatchBay::mouseDrag(const juce::MouseEvent &mousePosition)
{
    if (activeCableIndex.has_value()){
        patchCable[*activeCableIndex].setMousePosition(mousePosition);
        int inputIndex = patchCable[*activeCableIndex].getConnectionIndex(false);
        patchPoint[inputIndex].setIsAvailable(true);
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

void PatchBay::disconnectCable(int cableIdx, bool noTarget)
{
    patchCable[cableIdx].setCableConnected(false);
    patchCable[cableIdx].setCableInUse(false);
    patchPoint[cableIdx].setIsAvailable(true);

    int inputIdx = patchCable[cableIdx].getConnectionIndex(false);
    if (!noTarget && inputIdx >= 0)
        patchPoint[inputIdx].setIsAvailable(true);

    if (prevCableIndex.has_value())
    {
        patchPoint[*prevCableIndex].setIsAvailable(true);
        prevCableIndex.reset();
    }

    clearParameterValues(cableIdx);
    activeCableIndex.reset();
}

void PatchBay::connectCableToInput(int outputIdx, int inputIdx)
{
    auto& cable = patchCable[outputIdx];
    auto inputPosition = patchPoint[inputIdx].getBounds().getCentre().toFloat();

    setParameterValues(outputIdx, inputIdx);

    cable.setCableConnected(true);
    cable.setCableInUse(true);
    cable.setConnectionIndex(outputIdx, inputIdx);
    cable.setInputPosition(inputPosition);

    patchPoint[inputIdx].setIsAvailable(false);
    patchPoint[outputIdx].setIsAvailable(false);

    patchPoint[inputIdx].setConnectionIndex(outputIdx);
    patchPoint[outputIdx].setConnectionIndex(outputIdx);

    if (prevCableIndex.has_value())
        patchPoint[*prevCableIndex].setIsAvailable(true);

    activeCableIndex.reset();
    prevCableIndex = inputIdx;
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
    return 0;
}

void PatchBay::refreshFromParameters()
{
    for (int i = 0; i < audioProcessor.getParameters().size(); ++i)
    {
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[i]))
        {
            float value = param->getValue(); // 0.0–1.0
            parameterValueChanged(i, value); // manually trigger
        }
    }
}

void PatchBay::parameterValueChanged(int parameterIndex, float newValue)
{
    const juce::SpinLock::ScopedLockType lock(pendingLock);
    pendingChanges.emplace_back(parameterIndex, newValue);
    triggerAsyncUpdate();
}


void PatchBay::handleAsyncUpdate()
{
    
    std::vector<std::pair<int, float>> updatesCopy;

    {
        const juce::SpinLock::ScopedLockType lock(pendingLock);
        updatesCopy.swap(pendingChanges); // safely move all pending updates
    }

    for (const auto& [parameterIndex, newValue] : updatesCopy)
    {
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                auto input = rangedParam->convertFrom0to1(newValue);
                juce::String parameterID = param->paramID;
                auto paramRange = audioProcessor.apvts.getParameterRange(parameterID);

                for (int point = 0; point < 20; ++point)
                {
                    if (patchPointLayout[point].paramID == parameterID)
                    {
                        int output = patchPointLayout[point].localOutputIndex;
                        setCableFromParameter(output - 1, input);
                        DBG("cable set from " << parameterID << " output: " << output - 1 << " input: " << input);
                    }
                }
            }
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
    prevInputIndex = inputIndex;
    
    activeCableIndex.reset();
    repaint();
}
