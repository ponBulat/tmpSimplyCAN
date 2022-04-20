#pragma once

#include <QObject>

#include "simply.h"

class SimplyCAN : public QObject
{
    Q_OBJECT

    QByteArray m_portName;

public:
    explicit SimplyCAN(QObject *parent = nullptr);
    ~SimplyCAN();

    void printInfo();

signals:

};
