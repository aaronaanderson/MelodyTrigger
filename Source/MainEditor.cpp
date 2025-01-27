#include "MainProcessor.h"
#include "MainEditor.h"
//==============================================================================
MainEditor::MainEditor (MainProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), 
      durationKnob (p.getVTS(), "Duration", "Duration")
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (durationKnob);
    addAndMakeVisible (triggerNoteChooserLabel);

    juce::StringArray items;
    for(int i = 0; i < 128; i++) items.add (juce::String (i));

    triggerNoteChooser.addItemList(items, 1);
    triggerNoteChooser.onChange = [&]()
        {
            processorRef.setTriggerNote (triggerNoteChooser.getSelectedItemIndex() );
            
        };
    
    auto* param = processorRef.getVTS().getParameter("TriggerNote");
    int note = (int)param->convertFrom0to1 (param->getValue());
    triggerNoteChooser.setSelectedItemIndex (note);
    addAndMakeVisible (triggerNoteChooser);

    processorRef.getVTS().addParameterListener ("TriggerNote", this);

    setResizable (true, true);
    setSize (240, 120);
}
MainEditor::~MainEditor()
{
    processorRef.getVTS().removeParameterListener ("TriggerNote", this);
}
//==============================================================================
void MainEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainEditor::resized()
{
    auto b = getLocalBounds();
    
    durationKnob.setBounds (b.removeFromLeft (b.getWidth() / 2));
    triggerNoteChooserLabel.setBounds (b.removeFromTop (40));
    triggerNoteChooser.setBounds (b.removeFromTop (40));
}
