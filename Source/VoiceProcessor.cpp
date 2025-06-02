
#include "VoiceProcessor.h"


void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    this->sampleRate = sampleRate;
    // impulse
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

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    renderOscillator(outputBuffer);
}

void SynthVoice::setStepIndex(int index)
{
    stepIndex = index;
}

void SynthVoice::renderOscillator(juce::AudioBuffer<float> buffer)
{
    // Channels
 //   auto channelDataL = buffer.getWritePointer(0);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel){
        auto channelData = buffer.getWritePointer(channel);

        
        for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            //********* normalized patch bay inputs *********//
            modParam[0].baseValue = Utility::scale(pitch[stepIndex], 30.0f, 1200.0f, 0.0f, 1.0f);
            modParam[1].baseValue = Utility::scale(tone[stepIndex], 0.0f, 8.0f, 0.0f, 1.0f);
            modParam[2].baseValue = Utility::scale(mod[stepIndex], -12.0f, 12.0f, 0.0, 1.0f);
            modParam[3].baseValue = 1.0f;
            
            modParam[4].baseValue = Utility::scale(tension, 0.0f, 100.0f, 0.0, 1.0f);
            modParam[5].baseValue = Utility::scale(inharmonicity, 0.0f, 100.0f, 0.0, 1.0f);
            modParam[6].baseValue = Utility::scale(position, 0.0f, 100.0f, 0.0, 1.0f);
            modParam[7].baseValue = Utility::scale(algorithm, 0.0f, 7.0f, 0.0, 1.0f);
            
            modParam[8].baseValue = Utility::scale(operatorLevel, 0.0f, 100.0f, 0.0, 1.0f);
            modParam[9].baseValue = Utility::scale(noiseLevel, 0.0f, 100.0f, 0.0, 1.0f);
            modParam[10].baseValue = Utility::scale(noiseFreq, 100.0f, 8000.0f, 0.0f, 1.0f);
            modParam[11].baseValue = Utility::scale(noiseBandwidth, 0.0f, 10.0f, 0.0f, 1.0f);
            
            modParam[12].baseValue = Utility::scale(0.0f, 0.0f, 10.0f, 0.0f, 1.0f);// feedback, not used yet.
            
            
            
            
            float pitchScaled = Utility::scale(modParam[0].getParamValue(), 0.0f, 1.0f, 30.0f, 1200.0f);
            float toneScaled = Utility::scale(modParam[1].getParamValue(), 0.0f, 1.0f, 0.0f, 8.0f);
            float modScaled = Utility::scale(modParam[2].getParamValue(), 0.0f, 1.0f, -12.0f, 12.0f);
            float inharmScaled = Utility::scale(modParam[5].getParamValue(), 0.0f, 1.0f, 0.5f, 2.0f);
            float operLevelScaled = modParam[8].getParamValue();
            float noiseLevelScaled = modParam[9].getParamValue();
            float operator3 = 0.0f;
            
            float envelope = ampEnvelope.generateEnvelope();
            float modEnvelope = std::pow(2.0f, envelope * modScaled/12.0f);
            
            // voice DSP
            noise.setFilter(noiseFreq, noiseBandwidth);
            float filteredNoise = noise.processNoiseGenerator() * noiseLevelScaled * envelope;
            
            op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
            float operator2 = op[1].processOperator() * operLevelScaled * envelope;
            
            op[0].setOperatorInputs(pitchScaled * modEnvelope, operator2, toneScaled);
            float operator1 = op[0].processOperator() * operLevelScaled * envelope;
            float operatorSum = operator1;
            float output = operator1 + filteredNoise;
            
            
            
            if (algorithm == 0){
                // op2->op1 + noise
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, operator2, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1;
                output = operator1 + filteredNoise;
                
            } else if (algorithm == 1){
                // op3->op1 + op2 + noise
                op[2].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator3 = op[2].processOperator() * operLevelScaled * envelope;
                
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, operator3, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1 + operator2;
                output = operatorSum + filteredNoise;
                
            } else if (algorithm == 2){
                // op3->op1, op3 -> op2 + noise
                op[2].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator3 = op[2].processOperator() * operLevelScaled * envelope;
                
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, operator3, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, operator3, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1 + operator2;
                output = operatorSum + filteredNoise;
                
                
            } else if (algorithm == 3){
                // op3->op2->op1 + noise
                op[2].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator3 = op[2].processOperator() * operLevelScaled * envelope;
                
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, operator3, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, operator2, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1;
                output = operator1 + filteredNoise;
                
            } else if (algorithm == 4){
                // noise->op1
                op[0].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, filteredNoise, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1;
                output = operator1;
                
            } else if (algorithm == 5){
                // noise->op2->op1
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, filteredNoise, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, operator2, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1;
                output = operator1;
                
            } else if (algorithm == 6){
                op[2].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator3 = op[2].processOperator() * operLevelScaled * envelope;
                
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, 0.0f, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                operatorSum = operator1 + operator2 + operator3;
                output = operatorSum + filteredNoise;
                
            } else if (algorithm == 7){
                op[2].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, 0.0f, toneScaled);
                operator3 = op[2].processOperator() * operLevelScaled * envelope;
                
                op[1].setOperatorInputs(pitchScaled * modEnvelope * inharmScaled, operator3, toneScaled);
                operator2 = op[1].processOperator() * operLevelScaled * envelope;
                
                op[0].setOperatorInputs(pitchScaled * modEnvelope, operator3 + operator2, toneScaled);
                operator1 = op[0].processOperator() * operLevelScaled * envelope;
                
                operatorSum = operator1 + operator2 + operator3;
                output = operatorSum + filteredNoise;
            }
            
            
            // write to output
            channelData[sample] = output;
        }
    }
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
        modParam[inputValue].overrideValue = modParam[outputValue].getParamValue();
        modParam[inputValue].isConnected = true;
        
    } else {
        modParam[inputValue].isConnected = false;
    }
}
