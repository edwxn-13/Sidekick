/*
 ==============================================================================
 
 SynthVoice.cpp
 Created: 16 Sep 2024 5:48:19pm
 Author:  Edwin Nwosu
 
 ==============================================================================
 */

#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *, int /*currentPitchWheelPosition*/)
{
    m_currentAngle = 0.0;
    m_level = velocity * 0.15;
    m_tailOff = 0.0;
    
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond/getSampleRate();
    
    m_angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

void SynthVoice::prepareToPlay(double sampleRate)
{
    m_adsr.setSampleRate(sampleRate);
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        if(m_tailOff == 0.0){ m_tailOff = 1.0; }
    }
    
    else
    {
        clearCurrentNote();
        m_angleDelta = 0.0;
    }
}

void SynthVoice::pitchWheelMoved(int) {}

void SynthVoice::controllerMoved(int, int) {}

void SynthVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if(m_angleDelta != 0.0)
    {
        if(m_tailOff > 0.0)
        {
            while(--numSamples >= 0)
            {
                auto currentSample = (float) ((4.0f/juce::MathConstants<double>::pi) *
                                              (std::sin (m_currentAngle)) *
                                              ((1.0f/3.0f) * std::sin (3 * m_currentAngle)) *
                                              ((1.0f/5.0f) * std::sin (5 * m_currentAngle)) *
                                              ((1.0f/7.0f) * std::sin (7 * m_currentAngle)) *
                                              ((1.0f/9.0f) * std::sin (9 * m_currentAngle))) * m_level * m_tailOff;
                
                for (auto i = outputBuffer.getNumChannels(); --i >= 0;) outputBuffer.addSample (i, startSample, currentSample);
                
                m_currentAngle += m_angleDelta;
                ++startSample;
                
                m_tailOff *= 0.99;
                
                if(m_tailOff <= 0.005)
                {
                    clearCurrentNote();
                    m_angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float) ((std::sin (m_currentAngle) + ((1/3) * std::sin (3 * m_currentAngle))) * m_level);
                
                for(auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);
                m_currentAngle += m_angleDelta;
                ++startSample;
            }
        }
    }
    
    //m_adsr.applyEnvelopeToBuffer(outputBuffer, startSample, numSamples);
}

void SynthVoice::update(const float attack, const float decay, const float sustain, const float release)
{
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    m_adsr.setParameters(adsrParams);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

