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
public:
    explicit Controller(QObject *parent = nullptr);

public slots:
    void onPlay(const QString &file);
    void onStop();
    void onStopped();

    bool isPlaying() const { return playing; }

signals:
    void stopped();
    void playingChanged();

private:
    std::unique_ptr<AudioPlayer> player;
    bool playing;
};

#endif // CONTROLLER_H
