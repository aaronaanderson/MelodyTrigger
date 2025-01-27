#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "TriggerProcessor.h"
//==============================================================================
class MainProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    MainProcessor();
    ~MainProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getVTS() { return valueTreeState; }
    void setTriggerNote (int newTrigger) 
    { 
        auto p = valueTreeState.getParameter("TriggerNote");
        p->setValueNotifyingHost ((float)newTrigger / 127.0f);
    }
private:
    juce::AudioProcessorValueTreeState valueTreeState;
    TriggerProcessor triggerProcessor;

    juce::AudioProcessorValueTreeState::ParameterLayout makeParameterLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainProcessor)
};

class NormalizedFloatParameter : public juce::AudioParameterFloat
{
public:
    NormalizedFloatParameter (juce::String parameterName, 
                              float defaultValue = 0.0f,
                              juce::String unit = "")
    : juce::AudioParameterFloat ({parameterName.removeCharacters(" "), 1}, 
                                 parameterName,
                                 {0.0f, 1.0f},  
                                 defaultValue,
                                 juce::AudioParameterFloatAttributes().withLabel (unit)
                                                                       .withStringFromValueFunction ([&](float v, int n){ juce::ignoreUnused (n); return juce::String (v, 3); }))
    {
        valueChanged (defaultValue);
    }
};
class RangedFloatParameter : public juce::AudioParameterFloat
{
public:
    RangedFloatParameter (juce::String parameterName, 
                          juce::NormalisableRange<float> pRange,
                          float defaultValue = 0.0f,
                          juce::String unit = "")
    : juce::AudioParameterFloat ({parameterName.removeCharacters(" "), 1}, 
                                 parameterName,
                                 pRange,  
                                 defaultValue,
                                 juce::AudioParameterFloatAttributes().withLabel (unit)
                                                                      .withStringFromValueFunction ([&](float v, int n){ juce::ignoreUnused (n); return juce::String (v, 3); }))
    {
        valueChanged (defaultValue);
    }
};