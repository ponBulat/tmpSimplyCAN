#pragma once

#include <QObject>
#include <QTimer>

#include "simply.h"

namespace Bulat {

    enum Board {
        TableT = 0x02, // стол
        Power  = 0x04, // плата подсветки, после сдвига = 00100
        LaserT = 0x0f, // ТВЕРДОТЕЛЬНЫЙ ЛАЗЕР
    };

    enum MSG {
        COMMANDW = 0x00, // формирование комманды для любой платы
        STATUSW  = 0x02, // ожидаем из сети CAN
        STATUSR  = 0x03  // записываем в CAN чтобы узнать статус платы
    };
}

class SimplyCAN : public QObject
{
    Q_OBJECT

    QByteArray m_portName;
    uint16_t m_bitrate;

    QTimer m_timer;

public:
    explicit SimplyCAN(QObject *parent = nullptr);
    ~SimplyCAN();

    bool initCan();

    void printInfo();

    void scanBoards();

signals:

private slots:
    void slotCheckCanMessage();

private:
    void parseMessageLaserT( can_msg_t &can_msg_rx );
};


