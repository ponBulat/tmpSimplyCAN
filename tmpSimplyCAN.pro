QT -= gui
QT += serialport # для использования simpleCAN

CONFIG += c++latest console

SOURCES += \
        main.cpp \
        mycan.cpp

HEADERS += \
    mycan.h

# подключим библиотеку simplyCAN от Ixxat
LIBS += -L$$PWD/3rdparty/Ixxat/lib -lsimplyCAN
INCLUDEPATH += $$PWD/3rdparty/Ixxat/include
DEPENDPATH += $$PWD/3rdparty/Ixxat/include
