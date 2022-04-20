#pragma once

#include <QObject>

#include "simply.h"

class SimplyCAN : public QObject
{
    Q_OBJECT

    QByteArray m_portName;
    uint16_t m_bitrate;

public:
    explicit SimplyCAN(QObject *parent = nullptr);
    ~SimplyCAN();

    void printInfo();

signals:

};
