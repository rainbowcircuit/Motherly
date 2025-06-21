#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ColorLookAndFeel.h"

class PatchPoint : public juce::Component
{
public:
    PatchPoint();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setLabelText(juce::String labelText);
    
    bool isMouseOverPoint(juce::Point<int> mousePos);
    bool isMouseOverPanel(juce::Point<int> mousePos);
    void setMouseOver(bool mouseOver);

    void setConnectionIndex(int index); // these are so we can retrieve output from any cable
    int getConnectionIndex();
    
    bool isAvailable();
    void setIsAvailable(bool availablity);
    void setIsInput(bool input);
    void setIsOutputInverted(bool invert);
    bool getIsOutputInverted()
    {
        return isInverted;
    }

    
private:
    juce::Label patchLabel;
    juce::String labelText;
    float x = 0.0f, y = 0.0f;
    int connectionIndex = -1;
    bool isInput = true, available = true, mouseOver = false, isInverted = false;
};

class PatchCable : public juce::Component
{
public:
    PatchCable();
    void paint(juce::Graphics& g) override;
    
    void setMousePosition(const juce::MouseEvent &m);
    void setOutputPosition(const juce::Point<float> &p);
    void setInputPosition(const juce::Point<float> &p);
    
    //********************

    void setCableInUse(bool inUse);
    bool isCableInUse();
    
    //********************

    void setCableConnected(bool isConnected);
    void setConnectionIndex(int output, int input);
    int getConnectionIndex(bool getOutput);
    bool isCableConnected();
    
    //********************

    void setYOffset(float yOffset);

private:
    int cableColor;
    float yOffset;
    juce::Point<float> mousePoint { 0, 0 };
    juce::Point<float> outputPoint { 0, 0 };
    juce::Point<float> inputPoint { 0, 0 };
    int cableOutput = -1, cableInput = -1;
    bool cableInUse = false, cableConnected = false;
};


class PatchBay : public juce::Component, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater
{
public:
    
    PatchBay(MotherlyAudioProcessor& p);
    ~PatchBay();
    //********************
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //********************
    void mouseDown(const juce::MouseEvent &m) override;
    void mouseUp(const juce::MouseEvent &m) override;
    void mouseDrag(const juce::MouseEvent &m) override;
    
    //********************
    void disconnectCable(int cableIdx, bool noTarget = false);
    void connectCableToInput(int outputIdx, int inputIdx);
    int getLocalIndex(int indexToFind, bool findInput);
    
    void setParameterValues(int output, int input);
    
    void setInvertParameterValues(int output)
    {
    //    int outputIndex = patchPointLayout[output].localOutputIndex;
        juce::String outputParam = patchPointLayout[output].invertParamID;
        if (outputParam == "n/a")
            return;
            
        if (auto* param = audioProcessor.apvts.getParameter(outputParam))
        {
            float currentValue = param->getValue(); // Normalised 0.0 to 1.0
            float newValue = (currentValue > 0.5f) ? 0.0f : 1.0f; // Flip boolean
            param->setValueNotifyingHost(newValue);
        }
    }
    
    void setInvertFromParameter(int output, bool invert)
    {
        patchPoint[output].setIsOutputInverted(invert);
    }
    
    void clearParameterValues(int output);
    void setCableFromParameter(int output, int input);
    
    
    void refreshFromParameters();

private:
    //********************
    std::vector<std::pair<int, float>> pendingChanges;
    juce::SpinLock pendingLock;
    
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    void handleAsyncUpdate() override;
    
    //********************

    struct patchPointValues
    {
        bool isInput;
        juce::String labelText;
        juce::String paramID;
        juce::String invertParamID;
        int localInputIndex; // if 0, its an output;
        int localOutputIndex; // if 0, its an input;

        void setValue(juce::AudioProcessorValueTreeState& apvts, float v)
        {
            auto* p = apvts.getParameter (paramID);
            jassert (p != nullptr);
            p->setValueNotifyingHost (
                p->getNormalisableRange().convertTo0to1 (v));
        }
    };
    
    std::array<patchPointValues, 20> patchPointLayout
    {{
        { false, "Pitch", "pbPitchOut"   , "pbPitchInvert", 0, 1 },
        { false, "Tone", "pbToneOut"     , "pbToneInvert", 0, 2 },
        { false, "EG", "pbEGOut"         , "pbEGInvert", 0, 3 },
        { false, "Rpt", "pbRepeatOut"    , "pbRepeatInvert", 0, 4 },

        { true,  "Pitch", "pbPitchIn"    , "n/a", 1, 0 },
        { true,  "Tone", "pbToneIn"      , "n/a", 2, 0 },
        { false,  "Step", "pbStepOut"    , "pbStepInvert", 0, 5 },
        { false,  "Rand", "pbRandOut"    , "pbRandInvert", 0, 6 },
        
        { true,  "Tens", "pbTensionIn"   , "n/a", 3, 0 },
        { true,  "Inhr", "pbInharmIn"    , "n/a", 4, 0 },
        { true,  "Pos", "pbPositionIn"   , "n/a", 5, 0 },
        { true,  "Algo", "pbAlgoIn"      , "n/a", 6, 0 },
        
        { false,  "VCA", "pbVCAOut"      , "pbVCAInvert", 0, 7 },
        { false,  "Nois", "pbNoiseOut"   , "pbNoiseInvert", 0, 8 },
        { true,  "N.Lv", "pbNoiseLevelIn", "n/a", 7, 0 },
        { true,  "N.Fq", "pbNoiseFreqIn" , "n/a", 8, 0 },

        { true, "Op1", "pbOper1In"       , "n/a", 9, 0 },
        { true, "Op2", "pbOper2In"       , "n/a", 10, 0 },
        { true, "Op3", "pbOper3In"       , "n/a", 11, 0 },
        { false,  "Whl", "pbMWheelOut"   , "pbMWheelInvert", 0, 9 },
    }};
    
    std::atomic<int> parameterIndexAtomic;
    std::atomic<float> newValueAtomic;
    std::optional<int> activeCableIndex, prevCableIndex;
    std::array<PatchPoint, 20> patchPoint;
    std::array<PatchCable, 20> patchCable;

    juce::Label patchBayLabel;
    
    MotherlyAudioProcessor& audioProcessor;
};

