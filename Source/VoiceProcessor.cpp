
#include "VoiceProcessor.h"

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    
    seq.prepareToPlay(sampleRate, samplesPerBlock);
    seq.reset();
    
    for(int i = 0; i < 3; i++)
    {
        op[i].prepareToPlay(sampleRate, samplesPerBlock, numChannels);
        op[i].resetPhase();
        op[i].resetAngle();
    }
    ns.prepareToPlay(sampleRate);
    ampEnvelope.setSampleRate(sampleRate);
    combFilter.prepareToPlay(sampleRate, samplesPerBlock, numChannels);
    
    // smoothening
    pitchSmooth.reset(sampleRate, 0.001f);
    toneSmooth.reset(sampleRate, 0.02f);
    inharmSmooth.reset(sampleRate, 0.001f);
    positionSmooth.reset(sampleRate, 0.001f);
    outputSmooth.reset(sampleRate, 0.001f);
    op0LevelSmooth.reset(sampleRate, 0.001f);
    op1LevelSmooth.reset(sampleRate, 0.001f);
    op2LevelSmooth.reset(sampleRate, 0.001f);
    noiseLevelSmooth.reset(sampleRate, 0.001f);
    noiseFreqSmooth.reset(sampleRate, 0.001f);
    randSmooth.reset(sampleRate/10.0f);
    isPrepared = true;
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) {}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if (!allowTailOff || !ampEnvelope.isActive())
    {
        clearCurrentNote();
        return;
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue) {}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1)
    {
        modWheelRawValue = newControllerValue;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    
    VoiceParams params = processParameters(numSamples);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        seq.runSequencer();
        stepIndex = seq.getStepIndex();
        
        float gate = seq.getGate();
        if (!active) gate = 0.0f;

        float envelope = ampEnvelope.generateEnvelope(gate);
        params.envelope = envelope;
        envRawValue = envelope;
        float modEnvExponent = envelope * modRawValues[stepIndex] * (1.0f / 12.0f);
        params.modEnvelope = std::exp2(modEnvExponent);
        params.pitch = pitchSmooth.getNextValue() * 1170.0f + 30.0f;
        params.tone = toneSmooth.getNextValue();

        float noiseFreqFrom0to1 = noiseFreqSmooth.getNextValue() * 7900.0f + 100;
        ns.setFilter(noiseFreqFrom0to1, 3.0f);

        float output = processSynthVoice(params);
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, sample, output * outputSmooth.getNextValue());
        }
    }
}

SynthVoice::VoiceParams SynthVoice::processParameters(float numSamples)
{
    float inharmFrom0to1 = inharmSmooth.skip(numSamples) * 2.5 + 0.5f;
    float positionFrom0to1 = positionSmooth.skip(numSamples) * 45.0f + 5.0f;
    combFilter.setDelayTime(positionFrom0to1);

    // rand
    float rand = generateRand();
    randSmooth.setTargetValue(rand);
    randRawValue = randSmooth.skip(numSamples);
    
    // algorithm
    float algoScaled = algoIn0to1 * 9.0f;
    int algoFrom0to1 = (int)std::floor(algoScaled) % 10;

    // operator levels
    float op0LevelFrom0to1 = op0LevelSmooth.skip(numSamples);
    float op1LevelFrom0to1 = op1LevelSmooth.skip(numSamples);
    float op2LevelFrom0to1 = op2LevelSmooth.skip(numSamples);
    float noiseLevelFrom0to1 = noiseLevelSmooth.skip(numSamples);

    VoiceParams params { 0.0f, 0.0f, 0.0f, 0.0f, inharmFrom0to1, algoFrom0to1, op0LevelFrom0to1, op1LevelFrom0to1, op2LevelFrom0to1, noiseLevelFrom0to1 };
        
    return params;
}


float SynthVoice::processSynthVoice(VoiceParams params)
{
    float output;
    switch((int)params.algorithm)
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
    vcaSignalRawValue = output * 0.5f + 0.5f;
    return output;
}


void SynthVoice::setSequencer(juce::AudioPlayHead &p, int rate, bool active)
{
    seq.updateTransport(&p);
    seq.setRate(rate);
    this->active = active;
}

void SynthVoice::setStepParameters(int index, float pitchValue, float toneValue, float modValue, float repeatValue)
{
    pitchRawValues[index] = pitchValue;
    toneRawValues[index] = toneValue;
    modRawValues[index] = modValue;
    repeatRawValues[index] = repeatValue;
    seq.setRepeat(index, repeatValue);

}

void SynthVoice::setGlobalParameters(float tensionValue, float inharmValue, float positionValue)
{
    tensionRawValue = tensionValue;
    setEnvelope();

    inharmonicityRawValue = inharmValue;
    positionRawValue = positionValue;
}

void SynthVoice::setVoiceLevels(float outputGainValue, float op0LevelValue, float op1LevelValue, float op2LevelValue, float noiseLevelValue, float noiseFreqValue)
{
    outputGainRawValue = juce::Decibels::decibelsToGain(outputGainValue);
    outputSmooth.setTargetValue(outputGainRawValue);
    
    op0LevelRawValue = op0LevelValue;
    op1LevelRawValue = op1LevelValue;
    op2LevelRawValue = op2LevelValue;

    noiseLevelRawValue = noiseLevelValue;
    noiseFreqRawValue = noiseFreqValue;

}

void SynthVoice::setEnvelope()
{
    float pitchFrom0to1 = (1.0f - pitchIn0to1) * 40.0f + 10.0f;
    float tensionFrom0to1 = tensionIn0to1 * 40.0f + 10.0f;
    float repeatScaling = 1.0f/(repeatRawValues[stepIndex] + 1);
    
    float decayTime = pitchFrom0to1 * tensionFrom0to1 * repeatScaling;
    ampEnvelope.setEnvelopeSlew(2.0f, decayTime);
}


void SynthVoice::triggerEnvelope(float gate)
{
    if (gate > 0.0f && previousGate <= 0.0f)
    {
        op[0].resetPhase();
        op[1].resetPhase();
        op[2].resetPhase();
        
    }
    previousGate = gate;
}

float SynthVoice::generateRand()
{
    static juce::Random rand;
    static float random = rand.nextFloat();

    if (rand.nextFloat() < 0.00001f)
    {
        random = rand.nextFloat();
    }
    return random * 2.0f - 1.0f;
}


void SynthVoice::setAlgorithm(int algorithmValue)
{
    algorithmRawValue = algorithmValue;
}

float SynthVoice::processAlgorithm0(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise * 0.5f + 0.5f;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    operator1 = combFilter.processComb(operator1);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 + noise;
}

float SynthVoice::processAlgorithm1(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    operator2 = combFilter.processComb(operator2);
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 + operator1) * 0.75f + noise;
}

float SynthVoice::processAlgorithm2(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    float operator1And2 = combFilter.processComb(operator1 + operator2);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1And2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0 + noise;
}

float SynthVoice::processAlgorithm3(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    float operator1AndNoise = combFilter.processComb(operator1 + noise);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1AndNoise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0;
}

float SynthVoice::processAlgorithm4(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    operator2 = combFilter.processComb(operator2);
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, noise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 + operator1) * 0.75f;
}

float SynthVoice::processAlgorithm5(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    float operator1And2 = combFilter.processComb(operator1 + operator2);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1And2 + noise, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0;
}

float SynthVoice::processAlgorithm6(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, 0.0f, 0.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2 + noise, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    operator1 = combFilter.processComb(operator1);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0;
}

float SynthVoice::processAlgorithm7(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise * 3.0f, 1.0f);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    operator2 = combFilter.processComb(operator2);
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return (operator0 + operator1) * 0.75f;
}

float SynthVoice::processAlgorithm8(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise * 3.0f, p.tone);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, 0.0f, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    float operator1And2 = combFilter.processComb(operator1 + operator2);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1And2, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0;
}

float SynthVoice::processAlgorithm9(VoiceParams p)
{
    // scaling to 0.75 to reduce aliasing on 2nd order modulation
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;
    op[2].setOperatorInputs(p.pitch * p.modEnvelope, noise * 3.0f, p.tone);
    float operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    op[1].setOperatorInputs(p.pitch * p.inharm * p.modEnvelope, operator2, p.tone);
    float operator1 = op[1].processOperator() * p.envelope * p.op1Level;
    operator1 = combFilter.processComb(operator1);
    op[0].setOperatorInputs(p.pitch * p.modEnvelope, operator1, p.tone);
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    return operator0;
}

void SynthVoice::paramsIn0to1()
{
    // outputs
    float pitchIn0to1 = Utility::scale(pitchRawValues[stepIndex], 30.0f, 1200.0f, 0.0f, 1.0f);
    float toneIn0to1 = toneRawValues[stepIndex]/100.0f;
    float envIn0to1 = envRawValue;
    float repeatIn0to1 = repeatRawValues[stepIndex]/4.0f;
    float stepIn0to1 = Utility::scale(stepIndex, 0.0f, 7.0f, 0.0, 1.0f);
    float randIn0to1 = randRawValue;
    float vcaSignalIn0to1 = vcaSignalRawValue; // bipolar
    float noiseSignalIn0to1 = noiseSignalRawValue; // bipolar
    float modWheelIn0to1 = modWheelRawValue/127.0f; // unipolar

    // other defaults
    float tensionIn0to1 = tensionRawValue/100.0f;
    float inharmIn0to1 = inharmonicityRawValue/100.0f;
    float positionIn0to1 = positionRawValue/100.0f;
    float algoIn0to1 = Utility::scale(algorithmRawValue, 0.0f, 9.0f, 0.0f, 1.0f);
    float noiseLevelIn0to1 = noiseLevelRawValue/100.0f;
    float noiseFreqIn0to1 = Utility::scale(noiseFreqRawValue, 0.0f, 100.0f, 0.0f, 1.0f);
    float op1In0to1 = op0LevelRawValue/100.0f;
    float op2In0to1 = op1LevelRawValue/100.0f;
    float op3In0to1 = op2LevelRawValue/100.0f;
    
    outputsIn0to1 = { pitchIn0to1, toneIn0to1, envIn0to1, repeatIn0to1, stepIn0to1, randIn0to1, vcaSignalIn0to1, noiseSignalIn0to1, modWheelIn0to1 };
    
    defaultsIn0to1  = { pitchIn0to1, toneIn0to1, tensionIn0to1, inharmIn0to1, positionIn0to1, algoIn0to1, noiseLevelIn0to1, noiseFreqIn0to1, op1In0to1, op2In0to1, op3In0to1 };
}

void SynthVoice::setDefaults()
{
    for (int i = 0; i < 11; i++)
    {
        inputsIn0to1[i] = defaultsIn0to1[i];
    }
}

void SynthVoice::overrideDefaults(int outputIndex, int inputIndex)
{
    if (inputIndex != 0)
    {
        inputsIn0to1[inputIndex - 1] = defaultsIn0to1[inputIndex - 1] + outputsIn0to1[outputIndex];
    }
}

void SynthVoice::newParamsIn0to1()
{
    pitchIn0to1 = inputsIn0to1[0];
    pitchSmooth.setTargetValue(pitchIn0to1);
    toneIn0to1 = inputsIn0to1[1];
    toneSmooth.setTargetValue(toneIn0to1);
    tensionIn0to1 = inputsIn0to1[2];
    inharmIn0to1 = inputsIn0to1[3];
    inharmSmooth.setTargetValue(inharmIn0to1);
    positionIn0to1 = inputsIn0to1[4];
    positionSmooth.setTargetValue(positionIn0to1);
    algoIn0to1 = inputsIn0to1[5];
    noiseLevelIn0to1 = inputsIn0to1[6];
    noiseLevelSmooth.setTargetValue(noiseLevelIn0to1);
    noiseFreqIn0to1 = inputsIn0to1[7];
    noiseFreqIn0to1 = juce::jlimit(0.0f, 1.0f, noiseFreqIn0to1);
    noiseFreqSmooth.setTargetValue(noiseFreqIn0to1);
    op0In0to1 = inputsIn0to1[8];
    op0LevelSmooth.setTargetValue(op0In0to1);
    op1In0to1 = inputsIn0to1[9];
    op1LevelSmooth.setTargetValue(op1In0to1);
    op2In0to1 = inputsIn0to1[10];
    op2LevelSmooth.setTargetValue(op2In0to1);
}
