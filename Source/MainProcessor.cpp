#include "MainProcessor.h"
#include "MainEditor.h"

//==============================================================================
MainProcessor::MainProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       valueTreeState (*this, nullptr, "NoteTrigger", makeParameterLayout()), 
       triggerProcessor (60)
{
    
}
MainProcessor::~MainProcessor()
{
}
//==============================================================================
const juce::String MainProcessor::getName() const
{
    return JucePlugin_Name;
}
bool MainProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool MainProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool MainProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double MainProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
int MainProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}
int MainProcessor::getCurrentProgram()
{
    return 0;
}
void MainProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}
const juce::String MainProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}
void MainProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}
//==============================================================================
void MainProcessor::prepareToPlay (double sr, int samplesPerBlock)
{
    triggerProcessor.prepare (sr);
    juce::ignoreUnused (samplesPerBlock);
}
void MainProcessor::releaseResources(){}
bool MainProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
   if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo() ||
       layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
       return false;

    return true;
}
void MainProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    triggerProcessor.setTriggerNote ((int)*valueTreeState.getRawParameterValue ("TriggerNote" ));
    triggerProcessor.setDuration (*valueTreeState.getRawParameterValue ("Duration"));
    triggerProcessor.process (midiMessages, buffer.getNumSamples());
}
//==============================================================================
bool MainProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MainProcessor::createEditor()
{
    return new MainEditor (*this);
}
//==============================================================================
void MainProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml (valueTreeState.state.createXml());
    copyXmlToBinary (*xml, destData);
}
void MainProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName("NoteTrigger"))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MainProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout MainProcessor::makeParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::NormalisableRange<float> range = {1.0f, 4096.0f, 128.0f};
    layout.add (std::make_unique<RangedFloatParameter> ("Duration", range, 128.0f));
    layout.add (std::make_unique<juce::AudioParameterInt> ( juce::ParameterID ("TriggerNote"), "Trigger Note", 0, 127, 21));
    return layout;
}