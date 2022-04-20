#include "mycan.h"

#include <QSerialPortInfo>
#include <QDebug>

MyCan::MyCan(QObject *parent) : QObject(parent)
  , m_portName{ "/dev/ttyACM0" }
{

    if( simply_open( m_portName ) ) {
        qInfo() << "simplyCAN open port" << m_portName;
    } else {
        qWarning() << "simplyCAN dont open port" << m_portName;
        return;
    }

    qInfo() << " ";

//    can_msg_t can_msg_tx;
//    uint32_t last_sent = 0;
//    simply_last_error_t error;
//    uint8_t result = 0;

}

MyCan::~MyCan()
{
    if(  simply_close() ) {
        qInfo() << "simply close";
    } else {
        qInfo() << "simply dont close";
    }
}

void MyCan::printInfo()
{
    identification_t ident_msg;

    if( simply_identify(&ident_msg) ) {
        qInfo() << "INFO: " ;
        qInfo() << "  product_string: " << reinterpret_cast<const char*>( ident_msg.product_string );
        qInfo() << "  serial number: " << reinterpret_cast<const char*>( ident_msg.serial_number );
        qInfo() << "  fw_version: " << reinterpret_cast<const char*>( ident_msg.fw_version ) ;
        qInfo() << "  hw_version: " << reinterpret_cast<const char*>( ident_msg.hw_version );
        qInfo() << "  product version: " << reinterpret_cast<const char*>( ident_msg.product_version );
        qInfo() << "";
    } else {
        qWarning() << "identify error";
    }
}
