#include <QCoreApplication>

#include "simplycan.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SimplyCAN simplyCan;

//    simplyCan.printInfo();

    a.exit(0);

    return a.exec();
}
