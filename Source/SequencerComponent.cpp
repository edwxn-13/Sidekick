//
//  SequencerComponent.cpp
//  Sidekick - App
//
//  Created by Edwin Nwosu on 16/09/2024.
//

#include "SequencerComponent.h"


SequencerComponent::SequencerComponent(PolySynth * synth) : m_synth(synth)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    std::vector<MidiMessage> whatever(64);
    m_synthSequence = whatever;
    
    setUpUI();
}

SequencerComponent::~SequencerComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void SequencerComponent::setUpUI()
{
    
    auto area = getLocalBounds();
    auto headerFooterHeight = 36;
    
    playButton.setBounds(area.removeFromTop(headerFooterHeight));
    pauseButton.setBounds(area.removeFromTop(headerFooterHeight));
    stopButton.setBounds(area.removeFromTop(headerFooterHeight));
    recordButton.setBounds(area.removeFromTop(headerFooterHeight));
    
    playButton.setButtonText("Play");
    pauseButton.setButtonText("Pause");
    stopButton.setButtonText("Stop");
    recordButton.setButtonText("Record");
    
    recordButton.setToggleable(true);
    recordButton.setColour(recordButton.buttonOnColourId, Colours::red);
    recordButton.setColour(recordButton.buttonColourId, Colours::black);

    playButton.onClick = [this] { play();};
    pauseButton.onClick = [this] { pause();};
    stopButton.onClick = [this] { stop();};
    
    recordButton.onClick = [this]
    {
        if(isRecording())
        {
            stopRecording();
        }
        else{startRecording();}
        
        std::cout << isRecording();
    };

    addAndMakeVisible(&playButton);
    addAndMakeVisible(&pauseButton);
    addAndMakeVisible(&stopButton);
    addAndMakeVisible(&recordButton);
    
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

void SequencerComponent::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message)
{
    if(isRecording())
    {
        m_synthSequence[s_data.current_step] = message;
    }

}

void SequencerComponent::setMidiInput (int index)
{
    auto list = juce::MidiInput::getAvailableDevices();
    
    auto newInput = list[index];
    
    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);
    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    lastInputIndex = index;
}

void SequencerComponent::handlePartialSysexMessage(MidiInput * source, const uint8 *messageData, 
                                                   int numBytesSoFar, double timestamp)
{
    
}

//==============================================================================
void SequencerComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.
    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void SequencerComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    bufferToFill.clearActiveBufferRegion();
}

void SequencerComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void SequencerComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour::fromRGB(255, 0, 0));
    
    // You can add your drawing code here!
}

void SequencerComponent::resized()
{
    setSize (getWidth(), getHeight());
    
    auto area = getLocalBounds();
    auto headerFooterHeight = 36;
    playButton.setBounds(area.removeFromTop(headerFooterHeight));
    pauseButton.setBounds(area.removeFromTop(headerFooterHeight));
    stopButton.setBounds(area.removeFromTop(headerFooterHeight));
    recordButton.setBounds(area.removeFromTop(headerFooterHeight));
    midiInputList.setBounds (area.removeFromBottom(headerFooterHeight));

}

PolySynth * SequencerComponent::getSynth()
{
    return m_synth;
}

void SequencerComponent::hiResTimerCallback()
{
    m_synth->getSynthesiser()->allNotesOff(0, true);

    if(s_data.current_step > s_data.loop_length - 1)
    {
        s_data.current_step = 0;
        if(isRecording()){s_data.recording = false;}
    }
    
    
    m_synth->getSynthesiser()->noteOn(0, m_synthSequence[s_data.current_step].getNoteNumber(), 12);
    
    printf("current step: %d\n",s_data.current_step);
    //printf("current note: %d\n",m_synthSequence[s_data.current_step]);

    s_data.current_step++;
}

void SequencerComponent::play()
{
    float milliseconds = (((60/s_data.m_bpm) * 4))/8 * 1000;
    startTimer(milliseconds);
}

void SequencerComponent::pause()
{
    stopTimer();
    m_synth->getSynthesiser()->allNotesOff(0, true);

}

void SequencerComponent::stop()
{
    stopTimer();
    s_data.current_step = 0;
    m_synth->getSynthesiser()->allNotesOff(0, true);

}

void SequencerComponent::startRecording()
{
    s_data.recording = true;
}

void SequencerComponent::stopRecording()
{
    s_data.recording = false;
}

bool SequencerComponent::isRecording()
{
    return s_data.recording;
}
