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
    
    void triggerNote(juce::MidiBuffer& midiBuffer, int samplePosition);
    void flushNote(juce::MidiBuffer& midiBuffer);
    void reset();
    int getStepIndex();
    void setParameters(int sequencerRate, int stepOffset, int maxStepCount, int sequencerMode);
    void runSequencer(juce::MidiBuffer& midiBuffer, int samplePosition);
    
private:
    ImpulseMetro counter;
    int samplePerBlock = 0;
    bool pendingNoteOff = false;
    int sequencerMode = 0;
    bool directionFlag = false;
    int currentStepIndex = 0, sequencerRate = 0, stepOffset = 0, maxStepCount = 0;
};
