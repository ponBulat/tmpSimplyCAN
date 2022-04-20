#ifndef MYCAN_H
#define MYCAN_H

#include <QObject>

#include "simply.h"

class MyCan : public QObject
{
    Q_OBJECT

    char *m_portName;

public:
    explicit MyCan(QObject *parent = nullptr);
    ~MyCan();

    void printInfo();

signals:

};

#endif // MYCAN_H
