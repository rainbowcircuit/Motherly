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
    this->sampleRate = sampleRate;
    this->samplePerBlock = samplePerBlock;
    counter.setSampleRate(sampleRate);
    counter.reset();
}

void StepSequencer::updateTransport(juce::AudioPlayHead* playhead)
{
    counter.getTransport(playhead);
}

void StepSequencer::suppressMIDIInput(juce::MidiBuffer& midiBuffer)
{
    juce::MidiBuffer filteredBuffer;
    for (const juce::MidiMessageMetadata metadata : midiBuffer)
    {
        auto message = metadata.getMessage();
        
        if(message.isNoteOn() || message.isNoteOff()) {
            // suppress note on/off
        } else {
            // pass on mod wheel or pitch bend message
            filteredBuffer.addEvent(message, metadata.samplePosition);
        }
    }
    midiBuffer.swapWith(filteredBuffer);
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
    currentStepIndex = 7;
}

int StepSequencer::getStepIndex()
{
    return currentStepIndex % 8;
}

void StepSequencer::setRate(int sequencerRate)
{
    this->sequencerRate = sequencerRate;
    counter.setRate(sequencerRate);
}

void StepSequencer::runSequencer()
{
    if (counter.getIsPlaying()){
        if (counter.getImpulse()){
            currentStepIndex += 1;
        }
    } else {
        reset();
    }
}

