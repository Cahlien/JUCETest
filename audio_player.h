#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QObject>
#include <QQmlEngine>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>


class AudioPlayer : public QObject, public juce::AudioAppComponent
{
    Q_OBJECT    
public:
    explicit AudioPlayer(const QString &filename, QObject *parent = nullptr);
    ~AudioPlayer() override;

    void play();
    void stop();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

signals:
    void finished();

private:
    QString filename;
    std::unique_ptr<juce::AudioFormatManager> m_formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> m_readerSource;
    std::unique_ptr<juce::AudioTransportSource> m_transportSource;
};

#endif // AUDIO_PLAYER_H
