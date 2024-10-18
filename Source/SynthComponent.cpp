/*
 ==============================================================================
 
 SynthComponent.cpp
 Created: 16 Sep 2024 6:14:23pm
 Author:  Edwin Nwosu
 
 ==============================================================================
 */

#include "SynthComponent.h"

SynthComponent::SynthComponent() : m_polySynth(m_keyboardState), m_keyboardComponent(m_keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    
    addAndMakeVisible(m_keyboardComponent);
    setAudioChannels (0, 2);
    setSize (600, 160);
    startTimer (400);
    
    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);
    
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    
    juce::StringArray midiInputNames;
    for (auto input : midiInputs)
        midiInputNames.add (input.name);
    
    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }
    
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);
}


SynthComponent::~SynthComponent()
{
    shutdownAudio();
}

void SynthComponent::resized()
{
    //m_keyboardComponent.setBounds (10, 10, (getWidth()) - 20, (getHeight()/2.5) - 20);
    midiInputList.setBounds (200, 10, getWidth() - 210, 20);
}

void SynthComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(221, 228, 237));
}

void SynthComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    m_polySynth.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SynthComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    m_polySynth.getNextAudioBlock(bufferToFill);
}

void SynthComponent::releaseResources()
{
    m_polySynth.releaseResources();
}

void SynthComponent::timerCallback()
{

    stopTimer();
}

PolySynth * SynthComponent::getSynth()
{
    return &m_polySynth;
}


void SynthComponent::setMidiInput (int index)
{
    auto list = juce::MidiInput::getAvailableDevices();
    
    deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier,
                                                 m_polySynth.getMidiCollector()); // [12]
    
    auto newInput = list[index];
    
    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);
    
    deviceManager.addMidiInputDeviceCallback (newInput.identifier, m_polySynth.getMidiCollector()); // [13]
    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);
    
    lastInputIndex = index;
}
