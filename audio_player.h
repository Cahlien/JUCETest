#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QObject>

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = nullptr);

signals:
};

#endif // AUDIO_PLAYER_H
