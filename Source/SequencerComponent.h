//
//  SequencerComponent.hpp
//  Sidekick - App
//
//  Created by Edwin Nwosu on 16/09/2024.
//

#pragma once

#include <JuceHeader.h>
#include "SynthComponent.h"
using namespace juce;


struct SequencerData
{
    float m_bpm = 120;
    int current_step = 0;
    int loop_length = 16;
    bool recording = false;
    
};
class SequencerComponent  : public juce::AudioAppComponent, private juce::HighResolutionTimer, private juce::MidiInputCallback
{
public:
    //==============================================================================
    SequencerComponent(PolySynth * synth);
    ~SequencerComponent() override;
    
    void setUpUI();
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void play();
    void pause();
    void stop();
    void startRecording();
    void stopRecording();
    
    bool isRecording();
    void setMidiInput (int index);

    
    //==============================================================================
    
    float getBPM();
    PolySynth * getSynth();
    
    //==============================================================================

    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) override;
    
    //==============================================================================
    
    void handlePartialSysexMessage(MidiInput * source, const uint8 *messageData, int numBytesSoFar, double timestamp) override;

private:
    
    void hiResTimerCallback() override;
    
    std::vector<MidiMessage> m_synthSequence;
    juce::TextButton playButton, pauseButton, stopButton, recordButton;
    juce::TextButton activeButton;
    //==============================================================================
    // Your private member variables go here...
    PolySynth * m_synth;
    SequencerData s_data;
    
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerComponent)
};

