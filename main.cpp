#include <QCoreApplication>

#include "mycan.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyCan myCan;

    myCan.printInfo();

    a.exit(0);

    return a.exec();
}
