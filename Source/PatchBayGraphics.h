#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

class PatchPoint : public juce::Component
{
public:
    PatchPoint();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setLabelText(juce::String labelText);
    
    bool isMouseOver(juce::Point<int> mousePos);
    void setMouseOver(bool mouseOver);
    
    void setConnectionIndex(int index); // these are so we can retrieve output from any cable
    int getConnectionIndex();
    
    bool isAvailable();
    void setIsAvailable(bool availablity);
    
    void setIsInput(bool input);
    
private:
    juce::Label patchLabel;
    juce::String labelText;
    float x = 0.0f, y = 0.0f;
    int connectionIndex = -1;
    bool isInput = true, available = true, mouseOver = false;
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
        { false, "Pitch", "pbPitchOut"   , 0, 1 },
        { false, "Tone", "pbToneOut"     , 0, 2 },
        { false, "EG", "pbEGOut"         , 0, 3 },
        { false, "Rpt", "pbRepeatOut"    , 0, 4 },

        { true,  "Pitch", "pbPitchIn"    , 1, 0 },
        { true,  "Tone", "pbToneIn"      , 2, 0 },
        { false,  "Step", "pbStepOut"    , 0, 5 },
        { false,  "Rand", "pbRandOut"   , 0, 6 },
        
        { true,  "Tens", "pbTensionIn"   , 3, 0 },
        { true,  "Inhr", "pbInharmIn"    , 4, 0 },
        { true,  "Pos", "pbPositionIn"   , 5, 0 },
        { true,  "Algo", "pbAlgoIn"      , 6, 0 },
        
        { false,  "VCA", "pbVCAOut"      , 0, 7 },
        { false,  "Nois", "pbNoiseOut"   , 0, 8 },
        { true,  "N.Lv", "pbNoiseLevelIn", 7, 0 },
        { true,  "N.Fq", "pbNoiseFreqIn" , 8, 0 },

        { true, "Op1", "pbOper1In"       , 9, 0 },
        { true, "Op2", "pbOper2In"       , 10, 0 },
        { true, "Op3", "pbOper3In"       , 11, 0 },
        { false,  "Whl", "pbMWheelOut"  , 0, 9 },
    }};
    
    std::atomic<int> parameterIndexAtomic;
    std::atomic<float> newValueAtomic;
    std::optional<int> activeCableIndex, prevCableIndex;
    std::array<PatchPoint, 20> patchPoint;
    std::array<PatchCable, 20> patchCable;

    juce::Label patchBayLabel;
    
    MotherlyAudioProcessor& audioProcessor;
};

