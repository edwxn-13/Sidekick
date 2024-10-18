/*
  ==============================================================================

    PolySynth.h
    Created: 16 Sep 2024 5:46:09pm
    Author:  Edwin Nwosu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class PolySynth : public juce::AudioSource
{
public:
    PolySynth(juce::MidiKeyboardState& keyState);
    void setUsingSound();
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    juce::MidiMessageCollector* getMidiCollector();
    
    void setActive();
    void set_inActive();
    juce::Synthesiser * getSynthesiser();

private:
    juce::MidiKeyboardState& m_keybaordState;
    juce::Synthesiser m_synth;
    juce::MidiMessageCollector midiCollector;
    
    bool isActive = false;
    

};
