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
    void mouseDrag(const juce::MouseEvent &m) override;
    void mouseUp(const juce::MouseEvent &m) override;
    
    //********************

    void refreshFromParameters()
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

private:
    void parameterValueChanged(int parameterIndex, float newValue) override
    {
        parameterIndexAtomic.store(parameterIndex);
        newValueAtomic.store(newValue);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
    
    void handleAsyncUpdate() override {
        int parameterIndex = parameterIndexAtomic.load();
        float newValue = newValueAtomic.load();
        
        juce::AudioProcessorParameter* parameter = audioProcessor.getParameters()[parameterIndex];
        auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(parameter);
        
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(paramWithID))
        {
            if (paramWithID != nullptr){
                auto input = rangedParam->convertFrom0to1(newValue);
                juce::String parameterID = paramWithID->paramID;
             //   int input = audioProcessor.apvts.getRawParameterValue(parameterID)->load();
                juce::NormalisableRange<float> paramRange = audioProcessor.apvts.getParameterRange(parameterID);
                
                for (int point = 0; point < 20; point++)
                {
                    if (patchPointLayout[point].paramID == parameterID) // this means its an output
                    {
                        int output = patchPointLayout[point].localOutputIndex;
                        setCableFromParameter(output - 1, input);
                        
                        DBG("cable set from" << parameterID << " output: " << output - 1 << " input: " << input);
                    }
                }
            }
        }
    }
    
    
    void setParameterValues(int output, int input);
    void clearParameterValues(int output);
    void setCableFromParameter(int output, int input);
    int getLocalIndex(int indexToFind, bool findInput);
    void clearUnusedConnection();

    
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
        { false, "Pitch", "pbPitchOut", 0, 1 },
        { false, "Tone", "pbToneOut", 0, 2 },
        { false, "Mod", "pbModOut", 0, 3 },
        { false, "Prob", "pbProbOut", 0, 4 },

        { true,  "Pitch", "pbPitchIn", 1, 0 },
        { true,  "Tone", "pbToneIn", 2, 0 },
        { false,  "EG", "pbEGOut", 0, 5 },
        { false,  "Step", "pbStepOut", 0, 6 },
        
        { true,  "Tens", "pbTensionIn", 3, 0 },
        { true,  "Inhr", "pbInharmIn", 4, 0 },
        { true,  "Pos", "pbPositionIn", 5, 0 },
        { true,  "Step", "pbStepIn", 6, 0 },
        
        { true,  "O.Lv", "pbOperLevelIn", 7, 0 },
        { true,  "N.Lv", "pbNoiseLevelIn", 8, 0 },
        { true,  "N.Fq", "pbNoiseFreqIn", 9, 0 },
        { true,  "N.Bw", "pbNoiseBWIn", 10, 0 },

        { true, "Algo", "pbAlgoIn", 11, 0 },
        { true, "VCA", "pbVCAIn", 12, 0 },
        { false, "M.Wh", "pbMWheelOut", 0, 7 },
        { false,  "P.Bd", "pbPBendOut", 0, 8 },
    }};
    
    std::atomic<int> parameterIndexAtomic;
    std::atomic<float> newValueAtomic;
    std::optional<int> activeCableIndex, prevCableIndex;
    std::array<PatchPoint, 20> patchPoint;
    std::array<PatchCable, 20> patchCable;

    juce::Label patchBayLabel;
    
    MotherlyAudioProcessor& audioProcessor;
};

