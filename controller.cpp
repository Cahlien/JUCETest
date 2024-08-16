#include "controller.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject{parent}
{}

void Controller::onPlay(const QString &file)
{
    qInfo().nospace() << "Controller:onPlay(" << file << ")";
}

void Controller::onPause()
{
    qInfo().nospace() << "Controller:onPause()";
}
