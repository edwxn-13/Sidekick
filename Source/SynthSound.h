/*
  ==============================================================================

    SynthSound.h
    Created: 16 Sep 2024 5:48:13pm
    Author:  Edwin Nwosu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound: public juce::SynthesiserSound
{
public:
    
    SynthSound(){}
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
    
};
