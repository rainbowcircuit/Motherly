#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

MotherlyAudioProcessor::MotherlyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{}

MotherlyAudioProcessor::~MotherlyAudioProcessor() {}

//==============================================================================

const juce::String MotherlyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MotherlyAudioProcessor::acceptsMidi() const
{
    return true;
}

bool MotherlyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MotherlyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MotherlyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MotherlyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MotherlyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MotherlyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MotherlyAudioProcessor::getProgramName (int index)
{
    return {};
}

void MotherlyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MotherlyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.addSound(new SynthSound());
    synth.addVoice(new SynthVoice());
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int v = 0; v < synth.getNumVoices(); v++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(v)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
//    demoMode.prepareToPlay(sampleRate);
}

void MotherlyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MotherlyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MotherlyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //************** Transport **************//
    float modWheel = Utility::getModWheelValue(midiMessages, lastModWheel);
    lastModWheel = modWheel;
    
    auto playhead = getPlayHead();
    const auto opt = playhead->getPosition();
    const auto& pos = *opt;
    
    if (pos.getIsPlaying()) { synth.noteOn(0, 62, 127); }
    else { synth.noteOff(0, 62, 0.0f, true); }
    int rate = apvts.getRawParameterValue("seqRate")->load();
    
    //************** Drum DSP **************//
    float tension = apvts.getRawParameterValue("tension")->load();
    float inharmonicity = apvts.getRawParameterValue("inharmonicity")->load();
    float position = apvts.getRawParameterValue("position")->load();
    int algorithm = apvts.getRawParameterValue("algorithm")->load();
    float op1Level = apvts.getRawParameterValue("op1Level")->load();
    float op2Level = apvts.getRawParameterValue("op2Level")->load();
    float op3Level = apvts.getRawParameterValue("op3Level")->load();
    float noiseLevel = apvts.getRawParameterValue("noiseLevel")->load();
    float noiseFreq = apvts.getRawParameterValue("noiseFreq")->load();
    float output = apvts.getRawParameterValue("outputGain")->load();
    bool active = apvts.getRawParameterValue("active")->load();

    // patch bay parameters
    std::array<juce::String, 9> pbParamID = { "Pitch", "Tone", "EG", "Repeat", "Step", "Rand", "VCA", "Noise", "MWheel" };

    if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(0)))
    {
        for (int step = 0; step < 8; step++){
            juce::String freqParamID = "freq" + juce::String(step);
            juce::String toneParamID = "tone" + juce::String(step);
            juce::String modParamID = "mod" + juce::String(step);
            juce::String probParamID = "repeat" + juce::String(step);

            float pitch = apvts.getRawParameterValue(freqParamID)->load();
            float tone = apvts.getRawParameterValue(toneParamID)->load();
            float mod = apvts.getRawParameterValue(modParamID)->load();
            float repeat = apvts.getRawParameterValue(probParamID)->load();
            
            voice->setSequencer(*getPlayHead(), rate, active);
            voice->setStepParameters(step, pitch, tone, mod, repeat);
        }
        
        // patch bay params
        voice->paramsIn0to1();
        voice->setDefaults();
        for (int output = 0; output < 9; ++output)
        {
            juce::String patchBayParamID = "pb" + pbParamID[output] + "Out";
            float input = apvts.getRawParameterValue(patchBayParamID)->load();
            
            juce::String invertParamID = "pb" + pbParamID[output] + "Invert";
            float invert = apvts.getRawParameterValue(invertParamID)->load();
            
            voice->overrideDefaults(output, input, invert);
            voice->newParamsIn0to1();
        }
        voice->setGlobalParameters(tension, inharmonicity, position);
        voice->setVoiceLevels(output, op1Level, op2Level, op3Level, noiseLevel, noiseFreq);
        voice->setAlgorithm(algorithm);
        voice->setModWheel(modWheel);
        
        // get step index;
        int stepIndex = voice->getStepIndex();
        stepIndexAtomic.store(stepIndex);

    }
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
//    demoMode.processDemoMode(buffer);
    midiMessages.clear();
}

//==============================================================================
bool MotherlyAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* MotherlyAudioProcessor::createEditor()
{
    return new MotherlyAudioProcessorEditor (*this, stepIndexAtomic, amplitudeAtomic);
}

//==============================================================================
void MotherlyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void MotherlyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
       if (xmlState == nullptr)
           return;
       const auto newTree = juce::ValueTree::fromXml(*xmlState);
       apvts.replaceState(newTree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MotherlyAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
MotherlyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    //********************* Step Parameters *********************//
    
    for (int step = 0; step < 8; ++step)
    {
        juce::String freqParamID = "freq" + juce::String(step);
        juce::String freqParamName = "Frequency " + juce::String(step);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { freqParamID, 1},
                                                               freqParamName,
                                                                juce::NormalisableRange<float> { 30.0f, 1200.0f, 0.1f, 0.5f },
                                                                220.0f));

        juce::String toneParamID = "tone" + juce::String(step);
        juce::String toneParamName = "Tone " + juce::String(step);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { toneParamID, 1},
                                                               toneParamName,
                                                                juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f, 0.5f },
                                                                10.0f, "%"));

        juce::String modParamID = "mod" + juce::String(step);
        juce::String modParamName = "Pitch Mod Amount " + juce::String(step);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { modParamID, 1},
                                                               modParamName,
                                                                juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f, 1.0f },
                                                                0.0f, "st"));
        
        juce::String repeatParamID = "repeat" + juce::String(step);
        juce::String repeatParamName = "Repeat " + juce::String(step);

        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { repeatParamID, 1},
                                                                repeatParamName,
                                                                juce::StringArray { "1", "2", "3", "4" }, 0));

    }
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "tension", 1},
                                                            "Tension",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f, 0.5f },
                                                            20.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "inharmonicity", 1},
                                                            "Inharmonicity",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f, 1.0f },
                                                            20.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "position", 1},
                                                            "Position",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f, 1.0f },
                                                            50.0f, "%"));
    
    //********************* Voice Architecture Parameters *********************//
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "op1Level", 1},
                                                            "Operator 1 Level",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f },
                                                            100.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "op2Level", 1},
                                                            "Operator 2 Level",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f },
                                                            100.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "op3Level", 1},
                                                            "Operator 3 Level",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f },
                                                            100.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "noiseLevel", 1},
                                                            "Noise Level",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f },
                                                            25.0f, "%"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "noiseFreq", 1},
                                                            "Noise Frequency",
                                                            juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1, 0.5f },
                                                            100.0f, "%"));
        
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "algorithm", 1},
                                                            "Algorithm",
                                                             0, 9, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "outputGain", 1},
                                                            "Output Gain",
                                                            juce::NormalisableRange<float> { -72.0f, 6.0f, 0.1f, 1.0f },
                                                            -6.0f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "active", 1 }, "Active", true));

    //********************* Sequencer Parameter *********************//

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "seqRate", 1},
                                                            "Sequencer Rate",
                                                            juce::StringArray { "1/2", "3/8", "1/4", "3/16", "1/8", "3/16", "1/16" }, 4));
    
    //********************* Patch Bay Parameters *********************//
    
    std::array<juce::String, 9> pbParamID = { "Pitch", "Tone", "EG", "Repeat", "Step", "Rand", "VCA", "Noise", "MWheel" };

    for (int output = 0; output < 9; ++output)
    {
        juce::String patchBayParamID = "pb" + pbParamID[output] + "Out";
        juce::String patchBayParamName = pbParamID[output] + " Out";
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { patchBayParamID, 1},
                                                                patchBayParamName,
                                                                juce::StringArray { "No Input", "Pitch In", "Tone In", "Tension in", "Inharm In", "Position In", "Algorithm In", "Noise Level In", "Noise Freq In", "Operator 1 In", "Operator 2 In", "Operator 3 In" }, 0));
        
        juce::String invertParamID = "pb" + pbParamID[output] + "Invert";
        juce::String invertParamName = pbParamID[output] + " Invert";

        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { invertParamID, 1 }, invertParamName, false));
    }
    
    return layout;
}
