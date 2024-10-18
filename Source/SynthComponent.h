/*
  ==============================================================================

    SynthComponent.h
    Created: 16 Sep 2024 6:14:23pm
    Author:  Edwin Nwosu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PolySynth.h"
class SynthComponent  : public juce::AudioAppComponent, private juce::Timer
{
public:
    //==============================================================================
    SynthComponent();
    ~SynthComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setMidiInput (int index);
    PolySynth * getSynth();

private:
    //==============================================================================
    // Your private member variables go here...
    void timerCallback() override;

    PolySynth m_polySynth;
    juce::MidiKeyboardState m_keyboardState;
    juce::MidiKeyboardComponent m_keyboardComponent;
    
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthComponent)
};

