
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
        float pitchFrom0to1 = pitchIn0to1 * 1170.0f + 30.0f;
        float inharmFrom0to1 = inharmIn0to1 * 2.5 + 0.5f;
        float toneFrom0to1 = toneIn0to1;

        // voice DSP
        noise.setFilter(noiseFreq, noiseBandwidth);

        op[2].setOperatorInputs(pitchFrom0to1 * modEnvelope, 0.0f, 0.0f);
        float operator2 = op[2].processOperator() * envelope;
        
        op[1].setOperatorInputs(pitchFrom0to1 * inharmFrom0to1 * modEnvelope, operator2, toneFrom0to1);
        float operator1 = op[1].processOperator() * envelope;
        
        op[0].setOperatorInputs(pitchFrom0to1 * modEnvelope, operator1, toneFrom0to1);

        float operator0 = op[0].processOperator() * envelope;
        float output = (operator0 + operator1)/2;
        
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

void SynthVoice::paramsIn0to1()
{
    // outputs
    float pitchIn0to1 = Utility::scale(pitch[stepIndex], 30.0f, 1200.0f, 0.0f, 1.0f);
    float toneIn0to1 = Utility::scale(tone[stepIndex], 0.0f, 100.0f, 0.0, 1.0f);
    float modIn0to1 = Utility::scale(mod[stepIndex], -12.0f, 12.0f, 0.0, 1.0f);
    float probIn0to1 = Utility::scale(prob[stepIndex], 0.0f, 100.0f, 0.0, 1.0f);
    float envIn0to1 = ampEnvelope.generateEnvelope();
    float stepIn0to1 = Utility::scale(stepIndex, 0.0f, 7.0f, 0.0, 1.0f);
    float modWheelIn0to1 = 0.0f;
    float pBendIn0to1 = 0.0f;

    // other defaults
    float tensionIn0to1 = Utility::scale(tension, 30.0f, 1200.0f, 0.0f, 1.0f);
    float inharmIn0to1 = Utility::scale(inharmonicity, 0.0f, 100.0f, 0.0f, 1.0f);
    float positionIn0to1 = Utility::scale(position, 0.0f, 100.0f, 0.0f, 1.0f);
    float operLevelIn0to1 = Utility::scale(operatorLevel, 0.0f, 100.0f, 0.0f, 1.0f);
    float noiseLevelIn0to1 = Utility::scale(noiseLevel, 0.0f, 100.0f, 0.0f, 1.0f);
    float noiseFreqIn0to1 = Utility::scale(noiseFreq, 0.0f, 100.0f, 0.0f, 1.0f);
    float noiseBandIn0to1 = Utility::scale(noiseBandwidth, 0.0f, 100.0f, 0.0f, 1.0f);
    float algoIn0to1 = Utility::scale(algorithm, 0.0f, 100.0f, 0.0f, 1.0f);

    outputsIn0to1  = { pitchIn0to1, toneIn0to1, modIn0to1, probIn0to1, envIn0to1, stepIn0to1, modWheelIn0to1, pBendIn0to1 };
    
    defaultsIn0to1  = { pitchIn0to1, toneIn0to1, tensionIn0to1, inharmIn0to1, positionIn0to1, stepIn0to1, operLevelIn0to1, noiseLevelIn0to1, noiseFreqIn0to1, noiseBandIn0to1, algoIn0to1, 0.0f };
    
}

void SynthVoice::setDefaults()
{
    for (int i = 0; i < 12; i++)
    {
        inputsIn0to1[i] = defaultsIn0to1[i];
    }
    
}

void SynthVoice::overrideDefaults(int outputIndex, int inputIndex)
{
    if (inputIndex != 0)
    {
        inputsIn0to1[inputIndex - 1] = outputsIn0to1[outputIndex];
    }
}

void SynthVoice::newParamsIn0to1()
{
    pitchIn0to1 = inputsIn0to1[0];
    toneIn0to1 = inputsIn0to1[1];
    tensionIn0to1 = inputsIn0to1[2];
    inharmIn0to1 = inputsIn0to1[3];
    positionInIn0to1 = inputsIn0to1[4];
    stepIn0to1 = inputsIn0to1[5];
    operLevelIn0to1 = inputsIn0to1[6];
    noiseLevelIn0to1 = inputsIn0to1[7];
    noiseFreqIn0to1 = inputsIn0to1[8];
    noiseBandIn0to1 = inputsIn0to1[9];
    algoIn0to1 = inputsIn0to1[10];
    vcaMixIn0to1 = inputsIn0to1[11];
}
