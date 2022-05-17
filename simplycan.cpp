#include "simplycan.h"

#include <QBitArray>
#include <QSerialPortInfo>
#include <QDebug>

SimplyCAN::SimplyCAN(QObject *parent) : QObject(parent)
  #ifdef Q_OS_WIN
  , m_portName{ "COM3" } // or COM1
  #else
  , m_portName{ "/dev/ttyACM0" }
  #endif
  , m_bitrate{ 125 }
{
    qDebug() << "=== AVAILABLE PORTS ======\n";


    // посомтрим в дебаге какие вообще порты есть
    for( const auto &serialPortInfo : QSerialPortInfo::availablePorts() ) {
        qDebug() << "description: "         << serialPortInfo.description();
        qDebug() << "portName: "            << serialPortInfo.portName();
        qDebug() << "manufacturer: "        << serialPortInfo.manufacturer();
        qDebug() << "productIdentifier: "   << serialPortInfo.productIdentifier();
        qDebug() << "serialNumber: "        << serialPortInfo.serialNumber();
        qDebug() << "vendorIdentifier: "    << serialPortInfo.vendorIdentifier();
        qDebug() << "";
    }

    qDebug() << "========================";

    connect( &m_timer, &QTimer::timeout, this, &SimplyCAN::slotCheckCanMessage );
    m_timer.setInterval(500);

    initCan();
}

SimplyCAN::~SimplyCAN()
{
    simply_stop_can();

    if( simply_close() ) {
        qInfo() << "simply close";
    } else {
        qInfo() << "simply dont close";
    }
}

bool SimplyCAN::initCan()
{
//    printStatus();

    if( !simply_open( m_portName.data() ) ) {
        qWarning() << "simplyCAN dont open port" << m_portName;
        return false;
    }

    qInfo() << "simplyCAN open port" << m_portName << '\n';

//    printStatus();

    printInfo();

    if( !simply_initialize_can( m_bitrate ) ) {
        qWarning() << "error initialize can with bitrade " << m_bitrate;
        simply_close();
    }

    qInfo() << "initialize can with bitrade " << m_bitrate;

//    printStatus();

    if( !simply_start_can() ) {
        qWarning() << "simplyCAN dont start";
        simply_close();
    }

    qInfo() << "simpleCAN start" << '\n';

//    printStatus();

    m_timer.start();

    scanBoards();

    return true;
}

void SimplyCAN::printInfo()
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

void SimplyCAN::printStatus()
{
    can_sts_t can_sts;

    qDebug() << "\n ======== STATUS ===========";

    if( simply_can_status(&can_sts) ) {

        qInfo() << "number of free elements in CAN message tx fifo: " << can_sts.tx_free;

        /* format and print CAN status */

        if (can_sts.sts & CAN_STATUS_RUNNING) {
            qInfo() << "RUNNING ";
        } else {
            qInfo() << "==== NO RUNNING ";
        }

        if (can_sts.sts & CAN_STATUS_RESET) {
            qInfo() << "RESET ";
        } else {
            qInfo() << "==== NO RESET ";
        }

        if (can_sts.sts & CAN_STATUS_BUSOFF) {
            qInfo() << "BUSOFF ";
        } else {
            qInfo() << "====== NO BUSOFF ";
        }

        if (can_sts.sts & CAN_STATUS_ERRORSTATUS) {
            qInfo() << "ERR ";
        } else {
            qInfo() << "===== NO ERR";
        }

        if (can_sts.sts & CAN_STATUS_RXOVERRUN) {
            qInfo() << "RxO ";
        } else {
            qInfo() << "===== NO RxO  ";
        }

        if (can_sts.sts & CAN_STATUS_TXOVERRUN) {
            qInfo() << "TxO ";
        } else {
            qInfo() << "===== NO TxO ";
        }

        if (can_sts.sts & CAN_STATUS_PENDING) {
            qInfo() << "PENDING ";
        } else {
            qInfo() << "===== NO PENDING ";
        }
    } else {
        printError();
        qWarning() << "status error";
    }
}

void SimplyCAN::printError()
{
    qDebug() << "Error: ";
    switch ( simply_get_last_error() ) {
        case SIMPLY_S_NO_ERROR :                qInfo() << "No error occurred"; break;
        case SIMPLY_E_SERIAL_OPEN :             qWarning() << "Unable to open the serial port"; break;
        case SIMPLY_E_SERIAL_ACCESS :           qWarning() << "Access on serial port denied"; break;
        case SIMPLY_E_SERIAL_CLOSED :           qWarning() << "Serial communication port is closed "; break;
        case SIMPLY_E_SERIAL_COMM :             qWarning() << "Serial communication error"; break;
        case SIMPLY_E_CMND_REQ_UNKNOWN :        qWarning() << "Command unknown on device"; break;
        case SIMPLY_E_CMND_RESP_TIMEOUT :       qWarning() << "Command response timeout reached"; break;
        case SIMPLY_E_CMND_RESP_UNEXPECTED :    qWarning() << "Unexpected command response received"; break;
        case SIMPLY_E_CMND_RESP_ERROR :         qWarning() << "Command response error"; break;
        case SIMPLY_E_INVALID_PROTOCOL_VERSION : qWarning() << "Invalid simplyCAN protocol version"; break;
        case SIMPLY_E_INVALID_FW_VERSION :      qWarning() << "Invalid device firmware version"; break;
        case SIMPLY_E_INVALID_PRODUCT_STRING :  qWarning() << "Invalid simplyCAN product string"; break;
        case SIMPLY_E_CAN_INVALID_STATE :       qWarning() << "Invalid CAN state"; break;
        case SIMPLY_E_CAN_INVALID_BAUDRATE :    qWarning() << "Invalid CAN baud-rate"; break;
        case SIMPLY_E_TX_BUSY :                 qWarning() << "Message could not be sent. TX is busy"; break;
        case SIMPLY_E_API_BUSY :                qWarning() << " API is busy"; break;
        default: qWarning() << "unknow error";
    }
}

void SimplyCAN::scanBoards()
{
    can_msg_t can_msg_tx_LaserT;
    can_msg_t can_msg_tx_TableT;

    /* generate tx CAN message */
    can_msg_tx_LaserT.ident = ( Bulat::Board::LaserT << 6 | Bulat::MSG::STATUSR  );
    can_msg_tx_LaserT.dlc = 0;

    can_msg_tx_TableT.ident = ( Bulat::Board::TableT << 6 | Bulat::MSG::STATUSR  );
    can_msg_tx_TableT.dlc = 0;


    if( simply_send( &can_msg_tx_LaserT) ) {
        qInfo() << "simply send LaserT STATUSR";
    } else {
        qWarning() << "ERROR simply send LaserT STATUSR";
    }

    if( simply_send( &can_msg_tx_TableT) ) {
        qInfo() << "simply send TableT STATUSR";
    } else {
        qWarning() << "ERROR simply send TableT STATUSR";
    }
}

void SimplyCAN::slotCheckCanMessage()
{
    can_msg_t can_msg_rx;

    auto result = simply_receive(&can_msg_rx);

    if( result == 1 ) {
        qInfo() << "\nNew message:";
        const auto boardName = can_msg_rx.ident >> 6;

        switch ( boardName ) {
        case Bulat::Board::LaserT :
            parseMessageLaserT( can_msg_rx );
            break;
        case Bulat::Board::TableT :
            parseMessageTableT( can_msg_rx );
            break;
        default:
            break;
        }
    } else if ( result == -1 ) {
        qWarning() << "error result";
    }

//    printStatus();
//    printError();
}

void SimplyCAN::parseMessageLaserT( can_msg_t &can_msg_rx )
{
    qInfo() << "Board: LaserT";

    // маска 0x3f это 000 0011 1111
    // в предположении, что id 11-bit
    const auto idWithoutName = can_msg_rx.ident & 0x3f;

    switch ( idWithoutName ) {
    case Bulat::MSG::COMMANDW : {
        // ничего не делаем
        break;
    }
    case Bulat::MSG::STATUSW : {

        // ожидаем 4 байт
        const QBitArray status{ QBitArray::fromBits( reinterpret_cast<const char*>( can_msg_rx.payload ), can_msg_rx.dlc * 8 ) };

        qDebug() << "on: " << status.testBit( 0 );
        qDebug() << "studio: " << status.testBit( 1 );

        break;
    }
    case Bulat::MSG::STATUSR : {
        // ничего не делаем
        break;
    }
    }
}

void SimplyCAN::parseMessageTableT(can_msg_t &can_msg_rx)
{
    qInfo() << "Board: TableT";

    // маска 0x3f это 000 0011 1111
    // в предположении, что id 11-bit
    const auto idWithoutName = can_msg_rx.ident & 0x3f;

    switch ( idWithoutName ) {
    case Bulat::MSG::COMMANDW : {
        // ничего не делаем
        break;
    }
    case Bulat::MSG::STATUSW : {

        // проверим поля статус и студия в первом байте
        const QBitArray status{ QBitArray::fromBits( reinterpret_cast<const char*>( can_msg_rx.payload ), 8 ) };

        qDebug() << "on: " << status.testBit( 0 );
        qDebug() << "studio: " << status.testBit( 1 );

        break;
    }
    case Bulat::MSG::STATUSR : {
        // ничего не делаем
        break;
    }
    }
}
