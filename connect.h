#ifndef CONNECT_H
#define CONNECT_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class connect
{
public:
    connect();

private:
    QSerialPort *serial;

};

#endif // CONNECT_H
