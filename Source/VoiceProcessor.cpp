
#include "VoiceProcessor.h"


void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    ampEnvelope.setSampleRate(sampleRate);

    for(int i = 0; i < 3; i++)
    {
        op[i].prepareToPlay(sampleRate, samplesPerBlock, numChannels);
        op[i].reset();
    }
    noise.prepareToPlay(sampleRate);
    isPrepared = true;
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}


void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    ampEnvelope.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    ampEnvelope.noteOff();

    if (!allowTailOff || !ampEnvelope.isActive())
    {
        clearCurrentNote();
        return;
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        float envelope = ampEnvelope.generateEnvelope();
        float modEnvelope = std::pow(2.0f, envelope * mod[stepIndex]/12.0f);

        // voice DSP
        noise.setFilter(noiseFreq, noiseBandwidth);
        
        op[0].setOperatorInputs(pitch[stepIndex] * modEnvelope, 0.0f, 0.0f);
        float operator1 = op[0].processOperator() * envelope;
        float output = operator1;
        



        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.setSample(channel, sample, output);
            
        }
    }
}

void SynthVoice::setStepIndex(int index)
{
    stepIndex = index;
}

void SynthVoice::setStepParameters(int index, float pitchValue, float toneValue, float modValue, float probValue)
{
    pitch[index] = pitchValue;
    tone[index] = toneValue;
    mod[index] = modValue;
}

void SynthVoice::setGlobalParameters(float tensionValue, float inharmValue, float positionValue)
{
    position = positionValue;
    inharmonicity = inharmValue;
    tension = tensionValue;
    setEnvelope();
}

void SynthVoice::setVoiceLevels(float operatorLevelValue, float noiseLevelValue, float noiseFreqValue, float noiseBandwidthValue)
{
    operatorLevel = operatorLevelValue/100.0f;
    noiseLevel = noiseLevelValue/100.0f;
    noiseFreq = noiseFreqValue;
    noiseBandwidth = noiseBandwidthValue;
}

void SynthVoice::setAlgorithm(int algorithmValue)
{
    algorithm = algorithmValue;
}

void SynthVoice::setPatchBayParameters(int outputValue, int inputValue)
{
    if(inputValue > 0){
        
    } else {

    }
}
