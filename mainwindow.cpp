#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QPushButton>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_stule();

//    ui->edit_line->setText("11");
//    QObject::connect(send_message,SIGNAL(clicked(bool)),this,SLOT(slot_send_text()));

    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName("com4");//указали com-порт и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open((QIODevice::ReadWrite));//открыли порт
//    serial->write("Привет");//записываем данные

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
    printf(byteArrey.toHex());
    ui->label->setText("1234567890");//setText(byteArrey);//переводим в hex .toHex()
}

void MainWindow::on_send_message_clicked()
{
    QString sendMessage = ui->edit_line->text();
    QByteArray byteArrayMessage=sendMessage.toLocal8Bit();
    for (int i=0; i<=255; i++){
        byteArrayMessage[i] = i;
    }
    ui->label->setText(byteArrayMessage.toHex());
    serial->write(byteArrayMessage);//записываем данные
}

void MainWindow::set_stule()
{
    ui->send_message->setStyleSheet("QPushButton{"
                                "   color: #555555;"
                                "   background-color: white;"
                                "   border: lpx solid #828282;"
                                "   border-radius: 3px"
                                "}"
                                ""
                                "QPushButton:hover{"
                                "   border: lpx solid #828282;"
                                "   background-color: #d5d5d5;"
                                "}"
                                ""
                                "QPushButton:hover:pressed{"
                                "   background-color: gray;"
                                "   color: white;"
                                "}");
    ui->edit_line->setGeometry(QRect(10, 20, 191, 23));
    ui->edit_line->setStyleSheet("QLineEdit{"
                             "   color: #555555;"
                             "   background-color: white;"
                             "   border: lpx solid #828282;"
                             "   border-radius: 3px"
                             "}");
}

QString formatedEditLine = "";
QString odin = "0";
void MainWindow::on_edit_line_textChanged(const QString &arg1)
{
    int size=arg1.size();
    QString editedString = arg1[size-1];
    formatedEditLine = arg1;
    if (arg1.split(" ")[arg1.split(" ").size()-1].size() == 2){
        formatedEditLine += " ";
        ui->edit_line->setText(formatedEditLine);
    }
    QByteArray byteArrayMessage = editedString.toLocal8Bit();

    if (    (byteArrayMessage >= "0") && (byteArrayMessage <= "9") || (byteArrayMessage == "a") || (byteArrayMessage == "b") ||
            (byteArrayMessage == "c") || (byteArrayMessage == "d") || (byteArrayMessage == "e") ||
            (byteArrayMessage == "f") || (byteArrayMessage == " ") || (byteArrayMessage == "")){
        odin = "1";
        QString string = arg1;
        string.chop(1);
        if(string == " ")
        {
            ui->edit_line->setText(string);
        }
    } else {
        QString string = arg1;
        string.chop(1);
        ui->edit_line->setText(string);
        odin = "0";
    }

    sendByte(arg1);

//    ui->label->setText(formatedEditLine +"\n"+
//                       "последний символ: " + byteArrayMessage +"\n"+
//                       odin);//QString::number(byteArrayMessage.count())


}

void MainWindow::sendByte (QString arg1)
{
    QStringList listBytes = arg1.split(" ");
    QString editedArg = arg1;
    QByteArray byteArrayMessage;
    for (int i=0; i<=listBytes.size()-1; i++)
    {
//        byteArrayMessage[i] = listBytes[i].toInt(true, 16);
//        if(listBytes[i] == "af")
//        {
//            odin = listBytes[i][0];
//        } else {
//            odin = "0";
//        }

//        ui->label->setText(formatedEditLine +"\n"+
//                           "последний символ: " + byteArrayMessage +"\n"+
//                           odin);//QString::number(byteArrayMessage.count())


//        if(listBytes[i].size() == 1){
//            ui->label->setText(listBytes[i] + "0");
//        } else {
//            ui->label->setText(listBytes[i]);
//        }

    }
    serial->write(byteArrayMessage);
}
