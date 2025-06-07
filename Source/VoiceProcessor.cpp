
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
    ns.prepareToPlay(sampleRate);
    isPrepared = true;
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}


void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    ampEnvelope.noteOn();
    DBG("midi ON");
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    DBG("midi OFF");
    ampEnvelope.noteOff();

    if (!allowTailOff || !ampEnvelope.isActive())
    {
        clearCurrentNote();
        return;
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchWheel = newPitchWheelValue;
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1)
    {
        modWheel = newControllerValue;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        
        float envelope = ampEnvelope.generateEnvelope();
        float modEnvelope = std::pow(2.0f, envelope * mod[stepIndex]/12.0f);
        float pitchFrom0to1 = pitchIn0to1 * 1170.0f + 30.0f;
        float toneFrom0to1 = toneIn0to1;
        float inharmFrom0to1 = inharmIn0to1 * 2.5 + 0.5f;
        
        float op0LevelFrom0to1 = op0Level;
        float op1LevelFrom0to1 = op1Level;
        float op2LevelFrom0to1 = op2Level;
        float operLevelFrom0to1 = operLevelIn0to1;
        
        float noiseLevelFrom0to1 = noiseLevelIn0to1;
        float noiseFreqFrom0to1 = noiseFreqIn0to1 * 7900.0f + 100;
        float noiseBandFrom0to1 = noiseBandIn0to1 * 9.0f + 1.0f;
        ns.setFilter(noiseFreqFrom0to1, noiseBandFrom0to1);
        
        VoiceParams params { envelope, modEnvelope, pitchFrom0to1, toneFrom0to1, inharmFrom0to1, op0LevelFrom0to1, op1LevelFrom0to1, op2LevelFrom0to1, operLevelFrom0to1, noiseLevelFrom0to1 };
        
        float algoFrom0to1 = algoIn0to1 * 9.0f;
        algoFrom0to1 = std::floor(algoFrom0to1);

        float output;
        switch((int)algoFrom0to1)
        {
            case 0:
                output = processAlgorithm0(params);
                break;
            case 1:
                output = processAlgorithm1(params);
                break;
            case 2:
                output = processAlgorithm2(params);
                break;
            case 3:
                output = processAlgorithm3(params);
                break;
            case 4:
                output = processAlgorithm4(params);
                break;
            case 5:
                output = processAlgorithm5(params);
                break;
            case 6:
                output = processAlgorithm6(params);
                break;
            case 7:
                output = processAlgorithm7(params);
                break;
            case 8:
                output = processAlgorithm8(params);
                break;
            case 9:
                output = processAlgorithm9(params);
                break;
        }
        
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

void SynthVoice::setVoiceLevels(float outputGainValue, float op0LevelValue, float op1LevelValue, float op2LevelValue, float noiseLevelValue, float noiseFreqValue, float noiseBandwidthValue)
{
    outputGain = outputGainValue/100.0f;
    op0Level = op0LevelValue/100.0f;
    op1Level = op1LevelValue/100.0f;
    op2Level = op2LevelValue/100.0f;
    noiseLevel = noiseLevelValue/100.0f;
    noiseFreq = noiseFreqValue;
    noiseBandwidth = noiseBandwidthValue;
}

void SynthVoice::setAlgorithm(int algorithmValue)
{
    algorithm = algorithmValue;
}

float SynthVoice::processAlgorithm0(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 * p.operLevel) + noise;
}

float SynthVoice::processAlgorithm1(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return ((operator0 + operator1) * p.operLevel) + noise;
}

float SynthVoice::processAlgorithm2(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2 + operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 * p.operLevel) + noise;
}

float SynthVoice::processAlgorithm3(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1 + noise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 * p.operLevel;
}

float SynthVoice::processAlgorithm4(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2 + noise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 + operator1) * p.operLevel;
}

float SynthVoice::processAlgorithm5(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1 + operator2 + noise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 * p.operLevel;
}

float SynthVoice::processAlgorithm6(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2 + noise, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 * p.operLevel;
}

float SynthVoice::processAlgorithm7(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 + operator1) * p.operLevel;
}

float SynthVoice::processAlgorithm8(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1 + operator2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 * p.operLevel;
}

float SynthVoice::processAlgorithm9(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 * p.operLevel;
}

void SynthVoice::paramsIn0to1()
{
    // outputs
    float pitchIn0to1 = Utility::scale(pitch[stepIndex], 30.0f, 1200.0f, 0.0f, 1.0f);
    float toneIn0to1 = tone[stepIndex]/100.0f;
    float modIn0to1 = Utility::scale(mod[stepIndex], -12.0f, 12.0f, 0.0, 1.0f);
    float probIn0to1 = prob[stepIndex]/100.0f;
    float envIn0to1 = ampEnvelope.generateEnvelope();
    float stepIn0to1 = Utility::scale(stepIndex, 0.0f, 7.0f, 0.0, 1.0f);
    float modWheelIn0to1 = modWheel/127.0f;
    float pBendIn0to1 = Utility::scale(pitchWheel, 0.0f, 16383.0f, 0.0f, 1.0f);

    // other defaults
    float tensionIn0to1 = Utility::scale(tension, 5.0f, 100.0f, 0.0f, 1.0f);
    float inharmIn0to1 = inharmonicity/100.0f;
    float positionIn0to1 = position/100.0f;
    float operLevelIn0to1 = 1.0f;
    float noiseLevelIn0to1 = noiseLevel;
    float noiseFreqIn0to1 = Utility::scale(noiseFreq, 100.0f, 8000.0f, 0.0f, 1.0f);
    float noiseBandIn0to1 = noiseBandwidth/10.0f;
    float algoIn0to1 = Utility::scale(algorithm, 0.0f, 9.0f, 0.0f, 1.0f);

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

