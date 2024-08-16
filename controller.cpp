#include "controller.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject{parent}
    , player{}
    , playing{false}
{}

void Controller::onPlay(const QString &file)
{
    qInfo().nospace() << "Controller:onPlay(" << file << ")";
    player = std::make_unique<AudioPlayer>(file);
    connect(player.get(), &AudioPlayer::finished, this, &Controller::onStopped);
    player->play();
    playing = true;
}

void Controller::onStop()
{
    qInfo().nospace() << "Controller:onStop()";
    player->stop();
}

void Controller::onStopped()
{
    qInfo().nospace() << "Controller:onStopped()";
    playing = false;
    emit playingChanged();
    emit stopped();
}