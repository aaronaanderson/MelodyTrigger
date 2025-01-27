#pragma once 

#include <juce_audio_basics/juce_audio_basics.h>

struct Note
{
    Note (int note_number, int samples_remaining)
        : noteNumber (note_number),
          samplesRemaining (samples_remaining)
    {}
    int noteNumber;
    int samplesRemaining;
};
struct TriggerProcessor
{
    explicit TriggerProcessor (int trigger_note = 0)
        : triggerNote (trigger_note)
    {}
    void prepare (double sr)
    {
        sampleRate = sr;
    }
    void setTriggerNote (int newTriggerNote) {triggerNote = newTriggerNote;}
    void setDuration (float newDuration) {durationMS = newDuration;}
    void process (juce::MidiBuffer& buffer, int bufferSize)
    {
        triggeredNotes.clear();
        noteOffs.clear();

        // check for new note offs
        for (auto& an : activeNotes)
        {
            if (an.samplesRemaining < bufferSize)
            {
                noteOffs.addEvent (juce::MidiMessage::noteOff (1, an.noteNumber), an.samplesRemaining);
                activeNotes.remove(&an);
            } else {
                an.samplesRemaining -= bufferSize;
            }
        }
        // check for triggers and melody
        for (auto e : buffer)
        {
            auto m = e.getMessage();
            if (m.isNoteOn())
            {
                if (m.getNoteNumber() == triggerNote)

                    triggeredNotes.add (TriggeredNote ((int)m.getTimeStamp(), m.getVelocity()));
                else
                    melodyNotes.add ({m.getNoteNumber()});

            }
            if (m.isNoteOff())
            {
                melodyNotes.removeAllInstancesOf (m.getNoteNumber());
            }
        }
        
        buffer.clear();
        
        // add note offs
        for (auto e : noteOffs)
            if (e.getMessage().isNoteOff())
                buffer.addEvent(e.getMessage(), e.samplePosition);

        // add new triggered notes
        for (auto t : triggeredNotes)
        {
            for (auto mn : melodyNotes)
            {
                activeNotes.add (Note(mn, durationToSamples (durationMS) - (bufferSize - t.startSample)));
                buffer.addEvent (juce::MidiMessage::noteOn (1, mn, (juce::uint8)t.velocity), t.startSample);
            }
        }
    }
private:
    std::atomic<int> triggerNote;
    juce::Array<int> melodyNotes;
    juce::Array<Note> activeNotes; // array of notes triggered
    juce::MidiBuffer noteOffs;
    float durationMS {500.0f};
    double sampleRate {44100.0f};

    int durationToSamples (float duration_ms)
    {
        return static_cast<int> (duration_ms * 0.001f * sampleRate);
    }
    struct TriggeredNote
    {
        explicit TriggeredNote (int start_sample, int _velocity)
            :  startSample (start_sample),
               velocity (_velocity)
        {}
        int startSample; // 0 to BufferSize
        int velocity;
    };
    juce::Array<TriggeredNote> triggeredNotes;
};