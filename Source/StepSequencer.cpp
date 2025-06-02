/*
  ==============================================================================

    StepSequencer.cpp
    Created: 1 Jun 2025 6:28:14pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "StepSequencer.h"
void StepSequencer::prepareToPlay(double sampleRate, int samplePerBlock)
{
    this->samplePerBlock = samplePerBlock;
    counter.setSampleRate(sampleRate);
    counter.reset();
}

void StepSequencer::updateTransport(juce::AudioPlayHead* playhead)
{
    counter.getTransport(playhead);
}

// trying this out just briefly
void StepSequencer::triggerNote(juce::MidiBuffer& midiBuffer, int samplePosition)
{
    // run this method every sample.
    juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, 0, (juce::uint8)127);
    int noteOffSamplePosition = samplePosition + 100;

    // Add the noteOn at the current sample position
    midiBuffer.addEvent(noteOn, samplePosition);
    // Add the noteOff at the next sample position
    
    if (noteOffSamplePosition < samplePerBlock){

        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, 0);
        midiBuffer.addEvent(noteOff, noteOffSamplePosition);
    } else {
        pendingNoteOff = true;
    }
}

void StepSequencer::flushNote(juce::MidiBuffer& midiBuffer)
{
    if (pendingNoteOff)
    {
        midiBuffer.addEvent(juce::MidiMessage::noteOff(1, 0), 0); // start of next block
        pendingNoteOff = false;
    }
}

void StepSequencer::reset()
{
    currentStepIndex = 0;
}

int StepSequencer::getStepIndex()
{
    int stepIndex = (currentStepIndex + stepOffset);
    if(stepIndex >= maxStepCount)
    {
        currentStepIndex = stepOffset;
    }
    return stepIndex;
}

void StepSequencer::setParameters(int sequencerRate, int stepOffset, int maxStepCount, int sequencerMode)
{
    this->sequencerRate = sequencerRate;
    counter.setRate(sequencerRate);
    
    this->stepOffset = stepOffset;
    this->maxStepCount = maxStepCount;
    this->sequencerMode = sequencerMode;
}

void StepSequencer::runSequencer(juce::MidiBuffer& midiBuffer, int samplePosition)
{
    if (counter.getIsPlaying()){
        if (counter.getImpulse()){
            switch(sequencerMode){
                case 0: // forward
                    currentStepIndex += 1;
                    break;
                case 1: // backward
                    currentStepIndex--;
                    if (currentStepIndex < stepOffset)
                        currentStepIndex = maxStepCount - 1;
                    break;
                case 2: // ping pong
                    if (directionFlag) // forward
                        currentStepIndex++;
                    else // backward
                        currentStepIndex--;

                    if (currentStepIndex >= maxStepCount){
                        currentStepIndex = maxStepCount - 2;
                        directionFlag = false;
                    }
                    else if (currentStepIndex < stepOffset){
                        currentStepIndex = stepOffset + 1;
                        directionFlag = true;
                    }
                    break;
            }
            
            triggerNote(midiBuffer, samplePosition);
        }
    } else {
        reset();
    }
}
