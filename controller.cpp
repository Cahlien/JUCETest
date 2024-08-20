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

    // Create the AudioPlayer object
    player = std::make_unique<AudioPlayer>(file);

    // Connect signals from controller to player
    connect(player.get(), &AudioPlayer::finished, this, &Controller::onStopped);
    connect(this, &Controller::volumeChanged, player.get(), &AudioPlayer::setVolume);
    connect(this, &Controller::wetLevelChanged, player.get(), &AudioPlayer::setWetLevel);
    connect(this, &Controller::dryLevelChanged, player.get(), &AudioPlayer::setDryLevel);
    connect(this, &Controller::roomSizeChanged, player.get(), &AudioPlayer::setRoomSize);
    connect(this, &Controller::dampingChanged, player.get(), &AudioPlayer::setDamping);
    connect(this, &Controller::widthChanged, player.get(), &AudioPlayer::setWidth);
    connect(this, &Controller::freezeChanged, player.get(), &AudioPlayer::setFreeze);
    connect(this, &Controller::panChanged, player.get(), &AudioPlayer::setPan);

    // Immediately apply the current property values to the player
    player->setVolume(m_volume);
    player->setWetLevel(m_wetLevel);
    player->setDryLevel(m_dryLevel);
    player->setRoomSize(m_roomSize);
    player->setDamping(m_damping);
    player->setWidth(m_width);
    player->setFreeze(m_freeze);
    player->setPan(m_pan);

    // Start playback
    player->play();
    playing = true;
    emit playingChanged();
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

void Controller::setVolume(qreal volume)
{
    qInfo().nospace() << "Controller:setVolume(" << volume << ")";
    m_volume = volume;
    if(player != nullptr) {
        player->setVolume(m_volume);
    }
    emit volumeChanged(m_volume);
}

void Controller::setWetLevel(qreal wetLevel)
{
    qInfo().nospace() << "Controller:setWetLevel(" << wetLevel << ")";
    m_wetLevel = wetLevel;
    player->setWetLevel(m_wetLevel);
    emit wetLevelChanged(m_wetLevel);
}

void Controller::setDryLevel(qreal dryLevel)
{
    qInfo().nospace() << "Controller:setDryLevel(" << dryLevel << ")";
    m_dryLevel = dryLevel;
    player->setDryLevel(m_dryLevel);
    emit dryLevelChanged(m_dryLevel);
}

void Controller::setRoomSize(qreal roomSize) {
    qInfo().nospace() << "Controller:setRoomSize(" << roomSize << ")";
    m_roomSize = roomSize;
    player->setRoomSize(m_roomSize);
    emit roomSizeChanged(m_roomSize);
}

void Controller::setDamping(qreal damping)
{
    qInfo().nospace() << "Controller:setDamping(" << damping << ")";
    m_damping = damping;
    player->setDamping(m_damping);
    emit dampingChanged(m_damping);
}

void Controller::setWidth(qreal width)
{
    qInfo().nospace() << "Controller:setWidth(" << width << ")";
    m_width = width;
    player->setWidth(m_width);
    emit widthChanged(m_width);
}

void Controller::setFreeze(qreal freeze)
{
    qInfo().nospace() << "Controller:setFreeze(" << freeze << ")";
    m_freeze = freeze;
    player->setFreeze(m_freeze);
    emit freezeChanged(m_freeze);
}

void Controller::setPan(qreal pan)
{
    qInfo().nospace() << "Controller:setPan(" << pan << ")";
    m_pan = pan;
    player->setPan(m_pan);
    emit panChanged(m_pan);
}

