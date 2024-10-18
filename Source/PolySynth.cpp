/*
  ==============================================================================

    PolySynth.cpp
    Created: 16 Sep 2024 5:46:09pm
    Author:  Edwin Nwosu

  ==============================================================================
*/

#include "PolySynth.h"
#include "SynthVoice.h"

PolySynth::PolySynth(juce::MidiKeyboardState& keyState) : m_keybaordState(keyState)
{
    for (auto i = 0; i < 4; ++i)                // [1]
        m_synth.addVoice (new SynthVoice());
    m_synth.addSound (new SynthSound()) ;
}


void PolySynth::setUsingSound()
{
    m_synth.clearSounds();
}

void PolySynth::prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate)
{
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
    if(auto voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(0)))
    {
        voice->prepareToPlay(sampleRate);
        voice->update(0.1, 2.3, 1.0, 2.0);
    }
    midiCollector.reset (sampleRate);
}

void PolySynth::releaseResources()
{
    
}

void PolySynth::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    if(true)
    {
        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
        m_keybaordState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
        m_synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }
}

juce::MidiMessageCollector* PolySynth::getMidiCollector()
{
    return &midiCollector;
}

juce::Synthesiser * PolySynth::getSynthesiser()
{
    return &m_synth;
}

void PolySynth::setActive()
{
    isActive = true;
}
void PolySynth::set_inActive()
{
    isActive = false;
}


