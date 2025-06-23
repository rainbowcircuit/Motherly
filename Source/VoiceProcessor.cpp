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

    // smoothening
    pitchSmooth.reset(sampleRate, 0.02f);
    toneSmooth.reset(sampleRate, 0.02f);
    inharmSmooth.reset(sampleRate, 0.001f);
    positionSmooth.reset(sampleRate, 0.001f);
    outputSmooth.reset(sampleRate, 0.001f);
    op0LevelSmooth.reset(sampleRate, 0.05f);
    op1LevelSmooth.reset(sampleRate, 0.05f);
    op2LevelSmooth.reset(sampleRate, 0.05f);
    noiseLevelSmooth.reset(sampleRate, 0.05f);
    noiseFreqSmooth.reset(sampleRate, 0.01f);
    
    for(int i = 0; i < 4; i++)
    {
        op2GainInterpolation[i].reset(sampleRate, 0.001f);
        op1GainInterpolation[i].reset(sampleRate, 0.001f);
        op0GainInterpolation[i].reset(sampleRate, 0.001f);
        outGainInterpolation[i].reset(sampleRate, 0.001f);
    }

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

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    VoiceParams params = processParametersPerBlock(numSamples);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        seq.runSequencer();
        stepIndex = seq.getStepIndex();
        
        float gate = seq.getGate();
        if (!active) gate = 0.0f;
        onTrigger(gate);
        
        processParametersPerSample(params);
        
        float envelope = ampEnvelope.generateEnvelope(gate);
        params.envelope = envelope;
        envRawValue = envelope;
        float modEnvExponent = envelope * modRawValues[stepIndex] * (1.0f / 12.0f);
        params.modEnvelope = std::exp2(modEnvExponent);

        float output = processSynthVoice(params);
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, sample, output * outputSmooth.getNextValue());
        }
    }
}

SynthVoice::VoiceParams SynthVoice::processParametersPerBlock(float numSamples)
{
    VoiceParams params { };

    params.inharm = Utility::scale(inharmSmooth.skip(numSamples), -1.0f, 1.0f, 0.5f, 3.0f);
    params.position = Utility::scale(positionSmooth.skip(numSamples), -1.0f, 1.0f, 0.0f, 5.0f);
    
    // algorithm
    float algoScaled = algoIn0to1 * 9.0f;
    int algoFrom0to1 = (int)std::floor(algoScaled) % 10;
    setAlgorithmGain(algoFrom0to1);
    
    return params;
}

void SynthVoice::processParametersPerSample(SynthVoice::VoiceParams &params)
{
    params.pitch = Utility::scale(pitchSmooth.getNextValue(), -1.0f, 1.0f, 30.0f, 1200.0f);
    params.tone =  Utility::scale(toneSmooth.getNextValue(), -1.0f, 1.0f, 0.0f, 1.0f);


    float noiseFreqFrom0to1 = Utility::scale(noiseFreqSmooth.getNextValue(), -1.0f, 1.0f, 100.0f, 7900.0f);
    ns.setFilter(noiseFreqFrom0to1, 3.0f);
    
    
    params.op0Level = op0LevelSmooth.getNextValue();
    params.op1Level = op1LevelSmooth.getNextValue();
    params.op2Level = op2LevelSmooth.getNextValue();
    params.noiseLevel = noiseLevelSmooth.getNextValue();
    
}

float SynthVoice::processSynthVoice(VoiceParams p)
{
    float noise = ns.processNoiseGenerator() * p.envelope * p.noiseLevel;
    noiseSignalRawValue = noise;

    float pitch = p.pitch * p.modEnvelope;
    float tone = p.tone;
    float position = p.position;

    op[2].setOperatorInputs(pitch,
                             vcaSignalRawValue * op2GainInterpolation[0].getNextValue(), // feedback
                             0.0f,
                             0.0f,
                             noise * op2GainInterpolation[3].getNextValue(),
                             tone, position);
    operator2 = op[2].processOperator() * p.envelope * p.op2Level;
    
    op[1].setOperatorInputs(pitch * p.inharm,
                             operator0 * op1GainInterpolation[0].getNextValue(),
                             operator1 * op1GainInterpolation[1].getNextValue(),
                             operator2 * op1GainInterpolation[2].getNextValue(),
                             noise * op1GainInterpolation[3].getNextValue(),
                             tone, position);
    operator1 = op[1].processOperator() * p.envelope * p.op1Level;

    op[0].setOperatorInputs(pitch,
                             operator0 * op0GainInterpolation[0].getNextValue(),
                             operator1 * op0GainInterpolation[1].getNextValue(),
                             operator2 * op0GainInterpolation[2].getNextValue(),
                             noise * op0GainInterpolation[3].getNextValue(),
                             tone, 0.0f);
    
    float operator0 = op[0].processOperator() * p.envelope * p.op0Level;
    
    float output = (operator0 * outGainInterpolation[0].getNextValue()) +
                    (operator1 * outGainInterpolation[1].getNextValue()) +
                    (operator2 * outGainInterpolation[2].getNextValue()) +
                    (noise * outGainInterpolation[3].getNextValue());
    
    vcaSignalRawValue = output;
    return output;
}

int SynthVoice::getStepIndex()
{
    return stepIndex;
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
    float pitchFrom0to1 = Utility::scale(pitchIn0to1, -1.0f, 1.0f, 0.0f, 1.0f);
    float pitchScaled = (1.0f - pitchFrom0to1) * 50;
    float repeatScaled = 1.0f/(repeatRawValues[stepIndex] + 1);
    float tensionFrom0to1 = Utility::scale(tensionIn0to1, 0.0f, 1.0f, 10.0f, 50.0f);
    
    float activeDecayTime = pitchScaled * tensionFrom0to1 * repeatScaled;
    float inactiveDecayTime = 500.0f;
    float decayTime = active ? activeDecayTime : inactiveDecayTime;
    ampEnvelope.setEnvelopeSlew(2.0f, decayTime);
}

void SynthVoice::onTrigger(float gate)
{
    if (gate > 0.0f && previousGate <= 0.0f)
    {
        op[0].resetPhase();
        op[1].resetPhase();
        op[2].resetPhase();
        randRawValue = generateRand();
    }
    previousGate = gate;
}

void SynthVoice::setModWheel(float modWheel)
{
    modWheelRawValue = modWheel;
}

float SynthVoice::generateRand()
{
    float random = rand.nextFloat();
    return random * 2.0f - 1.0f;
}

void SynthVoice::setAlgorithm(int algorithmValue)
{
    algorithmRawValue = algorithmValue;
}

void SynthVoice::setAlgorithmGain(int algorithmIndex)
{
    float fb = feedbackPatched ? 1.0f : 0.0f;
    switch(algorithmIndex){
        case 0:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 1.0, 0.0};
            op0Gain = { 0.0, 1.0, 0.0, 0.0};
            outGain = { 1.0, 0.0, 0.0, 1.0};
            break;
        case 1:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 1.0, 0.0};
            op0Gain = { 0.0, 0.0, 0.0, 0.0};
            outGain = { 1.0, 1.0, 0.0, 1.0};
            break;
        case 2:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 0.0, 0.0};
            op0Gain = { 0.0, 1.0, 1.0, 0.0};
            outGain = { 1.0, 0.0, 0.0, 1.0};
            break;
        case 3:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 1.0, 0.0};
            op0Gain = { 0.0, 1.0, 0.0, 1.0};
            outGain = { 1.0, 0.0, 0.0, 0.0};
            break;
        case 4:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 1.0, 0.0};
            op0Gain = { 0.0, 0.0, 0.0, 1.0};
            outGain = { 1.0, 1.0, 0.0, 0.0};
            break;
        case 5:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 0.0, 0.0};
            op0Gain = { 0.0, 1.0, 1.0, 1.0};
            outGain = { 1.0, 0.0, 0.0, 0.0};
            break;
        case 6:
            op2Gain = { fb, 0.0, 0.0, 0.0};
            op1Gain = { 0.0, 0.0, 1.0, 1.0};
            op0Gain = { 0.0, 1.0, 0.0, 0.0};
            outGain = { 1.0, 0.0, 0.0, 0.0};
            break;
        case 7:
            op2Gain = { fb, 0.0, 0.0, 3.0};
            op1Gain = { 0.0, 0.0, 0.0, 0.0};
            op0Gain = { 0.0, 0.0, 1.0, 0.0};
            outGain = { 0.75, 0.75, 0.0, 0.0};
            break;
        case 8:
            op2Gain = { fb, 0.0, 0.0, 3.0};
            op1Gain = { 0.0, 0.0, 0.0, 0.0};
            op0Gain = { 0.0, 1.0, 1.0, 0.0};
            outGain = { 1.0, 0.0, 0.0, 0.0};
            break;
        case 9:
            op2Gain = { fb, 0.0, 0.0, 3.0};
            op1Gain = { 0.0, 0.0, 1.0, 0.0};
            op0Gain = { 0.0, 1.0, 0.0, 0.0};
            outGain = { 1.0, 0.0, 0.0, 0.0};
            break;
    }

    for (int i = 0; i < 4; i++)
    {
        op2GainInterpolation[i].setTargetValue(op2Gain[i]);
        op1GainInterpolation[i].setTargetValue(op1Gain[i]);
        op0GainInterpolation[i].setTargetValue(op0Gain[i]);
        outGainInterpolation[i].setTargetValue(outGain[i]);
    }
}

void SynthVoice::paramsIn0to1()
{
    // outputs
    float pitchIn0to1 = Utility::scale(pitchRawValues[stepIndex], 30.0f, 1200.0f, -1.0f, 1.0f);
    float toneIn0to1 = Utility::scale(toneRawValues[stepIndex], 0.0f, 100.0f, -1.0f, 1.0f);
    float envIn0to1 = envRawValue; // unipolar
    float repeatIn0to1 = Utility::scale(repeatRawValues[stepIndex], 1.0f, 4.0f, -1.0f, 1.0f);
    float stepIn0to1 = Utility::scale(stepIndex, 0.0f, 7.0f, -1.0, 1.0f);
    float randIn0to1 = randRawValue;
    float vcaSignalIn0to1 = vcaSignalRawValue;
    float noiseSignalIn0to1 = noiseSignalRawValue;
    float modWheelIn0to1 = Utility::scale(modWheelRawValue, 0.0f, 127.0f, 0.0, 1.0f);

    // other defaults
    float tensionIn0to1 = tensionRawValue/100.0f; // unipolar
    float inharmIn0to1 = Utility::scale(inharmonicityRawValue, 0.0f, 100.0f, -1.0f, 1.0f);
    float positionIn0to1 = Utility::scale(positionRawValue, 0.0f, 100.0f, -1.0f, 1.0f);
    float algoIn0to1 = algorithmRawValue/9.0f; // bipolar
    feedbackPatched = false;
    float noiseLevelIn0to1 = noiseLevelRawValue/100.0f; // unipolar
    float noiseFreqIn0to1 = Utility::scale(noiseFreqRawValue, 0.0f, 100.0f, -1.0f, 1.0f);
    float op1In0to1 = op0LevelRawValue/100.0f; // unipolar
    float op2In0to1 = op1LevelRawValue/100.0f; // unipolar
    float op3In0to1 = op2LevelRawValue/100.0f; // unipolar
    
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

void SynthVoice::overrideDefaults(int outputIndex, int inputIndex, bool outputInvertIndex)
{
    if (inputIndex != 0)
    {
        float inversion = outputInvertIndex ? -1.0f : 1.0f;
        inputsIn0to1[inputIndex - 1] = defaultsIn0to1[inputIndex - 1] + outputsIn0to1[outputIndex] * inversion;
    }
    
    if (outputIndex == 2 && outputInvertIndex){
        inputsIn0to1[inputIndex - 1] = defaultsIn0to1[inputIndex - 1] + (1.0f - outputsIn0to1[outputIndex]);
    }
    
    if (outputIndex == 6 && inputIndex == 1){
        inputsIn0to1[inputIndex - 1] = defaultsIn0to1[inputIndex - 1];
        feedbackPatched = true;
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
    noiseFreqIn0to1 = juce::jlimit(-1.0f, 1.0f, noiseFreqIn0to1);
    noiseFreqSmooth.setTargetValue(noiseFreqIn0to1);
    op0In0to1 = inputsIn0to1[8];
    op0LevelSmooth.setTargetValue(op0In0to1);
    op1In0to1 = inputsIn0to1[9];
    op1LevelSmooth.setTargetValue(op1In0to1);
    op2In0to1 = inputsIn0to1[10];
    op2LevelSmooth.setTargetValue(op2In0to1);
}
