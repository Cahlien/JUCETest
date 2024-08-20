#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QObject>
#include <QQmlEngine>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "panning_processor.h"


class AudioPlayer : public QObject, public juce::AudioAppComponent
{
    Q_OBJECT
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal wetLevel READ wetLevel WRITE setWetLevel NOTIFY wetLevelChanged)
    Q_PROPERTY(qreal dryLevel READ dryLevel WRITE setDryLevel NOTIFY dryLevelChanged)
    Q_PROPERTY(qreal roomSize READ roomSize WRITE setRoomSize NOTIFY roomSizeChanged)
    Q_PROPERTY(qreal damping READ damping WRITE setDamping NOTIFY dampingChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal freeze READ freeze WRITE setFreeze NOTIFY freezeChanged)
    Q_PROPERTY(qreal pan READ pan WRITE setPan NOTIFY panChanged)
public:
    explicit AudioPlayer(const QString &filename, QObject *parent = nullptr);
    ~AudioPlayer() override;

    void play();
    void stop();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    qreal volume() const;
    qreal wetLevel() const;
    qreal dryLevel() const;
    qreal roomSize() const;
    qreal damping() const;
    qreal width() const;
    qreal freeze() const;
    qreal pan() const;

    void setVolume(qreal volume);
    void setWetLevel(qreal wetLevel);
    void setDryLevel(qreal dryLevel);
    void setRoomSize(qreal roomSize);
    void setDamping(qreal damping);
    void setWidth(qreal width);
    void setFreeze(qreal freeze);
    void setPan(qreal pan);

public slots:
    void onReverbParametersChanged();

signals:
    void finished();
    void volumeChanged();
    void wetLevelChanged();
    void dryLevelChanged();
    void roomSizeChanged();
    void dampingChanged();
    void widthChanged();
    void freezeChanged();
    void panChanged();

private:
    QString m_filename;
    qreal m_volume;
    qreal m_wetLevel;
    qreal m_dryLevel;
    qreal m_roomSize;
    qreal m_damping;
    qreal m_width;
    qreal m_freeze;
    qreal m_pan;
    std::unique_ptr<juce::AudioFormatManager> m_formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> m_readerSource;
    std::unique_ptr<juce::AudioTransportSource> m_transportSource;
    std::unique_ptr<juce::Reverb> m_reverb;
    std::unique_ptr<juce::ReverbAudioSource> m_reverbSource;
    std::unique_ptr<PanningProcessor> m_panner;
};

#endif // AUDIO_PLAYER_H
