/*
  ==============================================================================

    SynthVoice.h
    Created: 16 Sep 2024 5:48:19pm
    Author:  Edwin Nwosu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"

struct SinOsc
{
    
};

class SynthVoice: public juce::SynthesiserVoice
{
public:
    SynthVoice();
    
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
     
    void stopNote (float /*velocity*/, bool allowTailOff) override;
    void pitchWheelMoved (int) override;
    void controllerMoved (int, int) override;
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    void update(const float attack, const float decay, const float sustain, const float release);
    void prepareToPlay(double sampleRate);


private:
    juce::dsp::Oscillator<float> m_oscillator { [](float x) {return x/juce::MathConstants<float>::pi;}};
    juce::ADSR m_adsr;
    double m_currentAngle = 0.0, m_angleDelta = 0.0, m_level = 0.0, m_tailOff = 0.0;
};
