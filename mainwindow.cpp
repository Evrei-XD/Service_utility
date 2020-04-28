#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName("com4");//указали com-порт и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open((QIODevice::ReadWrite));//открыли порт
    serial->write("Привет");//записываем данные

    connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecieve()));//соединяем чтение-приём данных

    //получим список доступных в системе com-портов при помощи QSerialPortInfo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort port;
        port.setPort(info);
        if (port.open(QIODevice::ReadWrite))
        {
            qDebug()<<"Название: " + info.portName() + " " + info.description() + info.manufacturer();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    //закрываем соединение при выходе
    serial->close();
    delete serial;
}

void MainWindow::serialRecieve()//получаем данные
{
    QByteArray byteArrey;//массив байт
    byteArrey = serial->readAll();//читаем всё
    ui->label->setText(byteArrey.toHex());//переводим в hex
}
