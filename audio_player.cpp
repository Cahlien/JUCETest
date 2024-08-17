#include "audio_player.h"
#include <QPromise>
#include <QtConcurrent>
#include <memory>
#include <juce_audio_processors/juce_audio_processors.h>

#define JUCE_USE_MP3AUDIOFORMAT 1

AudioPlayer::AudioPlayer(const QString &filename, QObject *parent)
    : QObject{parent}
    , juce::AudioAppComponent()
    , filename{filename}
    , m_formatManager{std::make_unique<juce::AudioFormatManager>()}
    , m_readerSource{nullptr}
    , m_transportSource{nullptr}
    , m_reverb{std::make_unique<juce::Reverb>()}
    , m_reverbSource{}
    , m_panner{std::make_unique<PanningProcessor>()}
{
    m_formatManager->registerBasicFormats();
    auto params{juce::Reverb::Parameters{}};
    params.roomSize = 0.8;
    params.damping = 0.1;
    params.wetLevel = 0.1;
    params.dryLevel = 0.9;
    params.width = 1.0;
    params.freezeMode = false;
    m_reverb->setParameters(params);
    m_panner->setPan(-1.0f);
    auto* reader = m_formatManager->createReaderFor(juce::File{filename.toStdString()});

    if (reader == nullptr)
    {
        juce::Logger::writeToLog("Failed to load audio file: " + filename.toStdString());
        return;
    }

    juce::Logger::writeToLog("Audio file loaded successfully: " + filename.toStdString());

    m_readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    m_transportSource = std::make_unique<juce::AudioTransportSource>();
    m_transportSource->setSource(m_readerSource.get());
    m_reverbSource = std::make_unique<juce::ReverbAudioSource>(m_transportSource.get(), false);
    m_reverbSource->setParameters(params);
    setAudioChannels(0, 2); // No input channels, 2 output channels
}

AudioPlayer::~AudioPlayer()
{
    if (m_transportSource->isPlaying())
    {
        m_transportSource->stop();
    }

    m_transportSource->setSource(nullptr);
    shutdownAudio();
}

void AudioPlayer::play()
{
    if (m_transportSource->isPlaying())
    {
        juce::Logger::writeToLog("Audio already playing.");
    }
    else
    {
        QtConcurrent::run([this]() {
            qInfo() << "Playing audio...";
            m_transportSource->setPosition(0.0);
            m_transportSource->start();
            while(m_transportSource->isPlaying())
            {
                QThread::msleep(100);
            }

            qInfo() << "Audio playback finished.";
            emit finished();
        });

        juce::Logger::writeToLog("Audio playback started.");
    }
}

void AudioPlayer::stop()
{
    if (m_transportSource->isPlaying())
    {
        // Stop the transport source
        m_transportSource->stop();
        juce::Logger::writeToLog("Audio playback stopped.");

        // Release resources and set the source to nullptr
        m_transportSource->setSource(nullptr);

        // Explicitly release resources
        m_transportSource->releaseResources();

        // Reset the reader source
        m_readerSource.reset();
    }
}

void AudioPlayer::releaseResources()
{
    juce::Logger::writeToLog("Releasing audio resources.");

    // Ensure the transport source is not requesting audio blocks
    if (m_transportSource != nullptr)
    {
        // Stop playback if it's still running
        if (m_transportSource->isPlaying())
        {
            m_transportSource->stop();
        }

        // Release resources and set the source to nullptr
        m_transportSource->releaseResources();
        m_transportSource->setSource(nullptr);
    }

    // Reset the reader source
    if (m_readerSource != nullptr)
    {
        m_readerSource.reset();
    }
}

void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::Logger::writeToLog("Preparing to play: Samples per Block = " +
                             juce::String(samplesPerBlockExpected) + ", Sample Rate = " + juce::String(sampleRate));
    m_transportSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_reverbSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_panner->prepareToPlay(sampleRate, samplesPerBlockExpected);
}

void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    juce::Logger::writeToLog("getNextAudioBlock called.");
    if (m_readerSource != nullptr && m_transportSource != nullptr)
    {
        m_reverbSource->getNextAudioBlock(bufferToFill);
        juce::dsp::AudioBlock<float> audioBlock(*bufferToFill.buffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);
        // Create an empty MidiBuffer (if not used) to pass to processBlock
        juce::MidiBuffer midiMessages;

        // Call the custom processBlock method
        m_panner->processBlock(*bufferToFill.buffer, midiMessages);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}
