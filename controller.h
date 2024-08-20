#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "audio_player.h"

#include <QObject>
#include <QQmlEngine>

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(qreal volume MEMBER m_volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal wetLevel MEMBER m_wetLevel READ wetLevel WRITE setWetLevel NOTIFY wetLevelChanged)
    Q_PROPERTY(qreal dryLevel MEMBER m_dryLevel READ dryLevel WRITE setDryLevel NOTIFY dryLevelChanged)
    Q_PROPERTY(qreal roomSize MEMBER m_roomSize READ roomSize WRITE setRoomSize NOTIFY roomSizeChanged)
    Q_PROPERTY(qreal damping MEMBER m_damping READ damping WRITE setDamping NOTIFY dampingChanged)
    Q_PROPERTY(qreal width MEMBER m_width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal freeze MEMBER m_freeze READ freeze WRITE setFreeze NOTIFY freezeChanged)
    Q_PROPERTY(qreal pan MEMBER m_pan READ pan WRITE setPan NOTIFY panChanged)
public:
    explicit Controller(QObject *parent = nullptr);

public slots:
    void onPlay(const QString &file);
    void onStop();
    void onStopped();

    bool isPlaying() const { return playing; }

    qreal volume() const { return m_volume; }
    qreal wetLevel() const { return m_wetLevel; }
    qreal dryLevel() const { return m_dryLevel; }
    qreal roomSize() const { return m_roomSize; }
    qreal damping() const { return m_damping; }
    qreal width() const { return m_width; }
    qreal freeze() const { return m_freeze; }
    qreal pan() const { return m_pan; }

    void setVolume(qreal volume);
    void setWetLevel(qreal wetLevel);
    void setDryLevel(qreal dryLevel);
    void setRoomSize(qreal roomSize);
    void setDamping(qreal damping);
    void setWidth(qreal width);
    void setFreeze(qreal freeze);
    void setPan(qreal pan);

signals:
    void stopped();
    void playingChanged();
    void volumeChanged(qreal volume);
    void wetLevelChanged(qreal wetLevel);
    void dryLevelChanged(qreal dryLevel);
    void roomSizeChanged(qreal roomSize);
    void dampingChanged(qreal damping);
    void widthChanged(qreal width);
    void freezeChanged(qreal freeze);
    void panChanged(qreal pan);

private:
    qreal m_volume = 1.0;
    qreal m_wetLevel = 0.33;
    qreal m_dryLevel = 0.4;
    qreal m_roomSize = 0.5;
    qreal m_damping = 0.5;
    qreal m_width = 1.0;
    qreal m_freeze = 0.0;
    qreal m_pan = 0.0;
    std::unique_ptr<AudioPlayer> player;
    bool playing = false;
};

#endif // CONTROLLER_H
