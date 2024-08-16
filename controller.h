#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QQmlEngine>

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Controller(QObject *parent = nullptr);

public slots:
    void onPlay(const QString &file);
    void onPause();

signals:
};

#endif // CONTROLLER_H
