/*
  ==============================================================================

    StepSequencer.h
    Created: 1 Jun 2025 6:28:14pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DSP.h"

class StepSequencer
{
public:
    void prepareToPlay(double sampleRate, int samplePerBlock);
    void updateTransport(juce::AudioPlayHead* playhead);
    void suppressMIDIInput(juce::MidiBuffer& midiBuffer);
    
    void flushNote(juce::MidiBuffer& midiBuffer);
    void reset();
    int getStepIndex();
    void setRate(int sequencerRate);
    void runSequencer();
    
    float getGate()
    {
        counter.setStepIndex(currentStepIndex % 8);
        return counter.getGate();
    }
    
    void setRepeat(int index, int repeatValue)
    { 
        counter.setRepeat(index, repeatValue);
    }
    
private:
    Metro counter;
    double sampleRate;
    int samplePerBlock = 0;
    bool pendingNoteOff = false, rateUpdatePending = false;
    int currentStepIndex = 0, sequencerRate = 0;

    
};
