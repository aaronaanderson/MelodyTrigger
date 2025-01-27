#pragma once

#include "MainProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
struct AttachedKnob : public juce::Component
{
    AttachedKnob (juce::AudioProcessorValueTreeState& vts, 
                    juce::String labelText, 
                    juce::String paramID)
    {
        label.setJustificationType (juce::Justification::centred);
        label.setText (labelText, juce::dontSendNotification);
        addAndMakeVisible (label);

        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 20);
        slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        addAndMakeVisible (slider);
        attachment.reset (new SliderAttachment (vts, paramID, slider));
    }
    void resized() override
    {
        auto b = getLocalBounds();
        int eighthHeight = b.getHeight() / 8;
        label.setBounds (b.removeFromTop (eighthHeight));
        slider.setBounds (b.removeFromTop (eighthHeight * 7));
    }
private:
    juce::Label label;
    juce::Slider slider;
    std::unique_ptr<SliderAttachment> attachment;
};

//==============================================================================
class MainEditor final : public juce::AudioProcessorEditor, 
                         public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit MainEditor (MainProcessor&);
    ~MainEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    //==============================================================================
    void parameterChanged (const juce::String& parameterID, float newValue) override
    {
        juce::ignoreUnused (newValue);
        auto* p = processorRef.getVTS().getParameter (parameterID);
        int note = (int)p->convertFrom0to1 (p->getValue());
        triggerNoteChooser.setSelectedItemIndex (note);
    }

private:
    MainProcessor& processorRef;
    AttachedKnob durationKnob;
    juce::Label    triggerNoteChooserLabel {"asdf;lkj", "Trigger Note"};
    juce::ComboBox triggerNoteChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainEditor)
};
