#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "LogCategories.h"
#include "constant.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFile>
#include <QMessageBox>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QtEndian>
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_stule();
    serialBuffer = "";
    productIdentifier = "";
    vendorIdentifier = "";
    QString logName = "Log_"+QDateTime::currentDateTime().toString("hh:mm:ss___dd.MM.yyyy") + ".txt";
//    QFile file(logName);
//    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
//        file.write("blablabla");
//        file.close();
//    }

//    ui->edit_line->setText("f0 ab 1d");
//    QObject::connect(send_message,SIGNAL(clicked(bool)),this,SLOT(slot_send_text()));

    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName("COM4");//указали com-порт
    serial->open((QIODevice::ReadWrite));//открыли и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    update_ui();//вызов повторяющейся функции

    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecieve()));//соединяем чтение-приём данных
//    connect(serial, SIGNAL(ReadPastEnd()), this, SLOT(serialRecieveFinish()));

    //получим список доступных в системе com-портов при помощи QSerialPortInfo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort port;
        port.setPort(info);
        if (port.open(QIODevice::ReadWrite))
        {
            qDebug()<<"Название: " + info.portName() + " " + info.description() + info.manufacturer();
        }

        if((info.hasProductIdentifier() || info.hasVendorIdentifier()))
        {
            qDebug()<<info.productIdentifier();
            qDebug()<<info.vendorIdentifier();
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


quint16 receiveVector[7];
QString reseiveMessage = "";
void MainWindow::serialRecieve()//получаем данные
{
    byteArreyReceiveMessage = serial->readAll();//читаем всё
//    byteArreyReceiveMessage[0] = 1;
//    byteArreyReceiveMessage[1] = 0;
//    byteArreyReceiveMessage[2] = 2;
//    byteArreyReceiveMessage[3] = 0;
//    byteArreyReceiveMessage[4] = 3;
//    byteArreyReceiveMessage[5] = 0;
//    byteArreyReceiveMessage[6] = 4;
//    byteArreyReceiveMessage[7] = 0;
//    byteArreyReceiveMessage[8] = 5;
//    byteArreyReceiveMessage[9] = 0;
//    byteArreyReceiveMessage[10] = 6;
//    byteArreyReceiveMessage[11] = 0;
//    byteArreyReceiveMessage[12] = 7;
//    byteArreyReceiveMessage[13] = 0;
    QDataStream dataStream(byteArreyReceiveMessage);
    serialBuffer = byteArreyReceiveMessage.toHex();

    for(int i=0; i<byteArreyReceiveMessage.size(); i+=2)
    {
        receiveVector[i/2] =qFromBigEndian<quint16>(((const uchar*)byteArreyReceiveMessage.constData()+i));
//        qDebug() << "int" + QString::number(i/2) + " = "+ QString::number(receiveVector[i/2]);
    }
    reseiveMessage = byteArreyReceiveMessage.toHex();
    QString oldLable = ui->label->text();
    ui->label->setText(oldLable+"<--- "+reseiveMessage +"\n");//QString::number(byteArrayMessage.count())
}

void MainWindow::on_send_message_clicked()
{
    QString sendMessage = ui->edit_line->text();
    QByteArray byteArrayMessage=sendMessage.toLocal8Bit();

    sendBytes(ui->edit_line->text());
}

void MainWindow::set_stule()
{
    ui->send_message->setStyleSheet(
                     "QPushButton{"
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
    ui->edit_line->setGeometry(QRect(10, 40, 211, 23));
    ui->edit_line->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->send_shake_time->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->send_cool_time->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->send_stop_current->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->send_stop_strength->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->connect_button->setStyleSheet(
                      "QPushButton{"
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
    ui->tenso_calib->setStyleSheet(
                    "QPushButton{"
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
    ui->connect_button->setStyleSheet(
                      "QPushButton{"
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
    ui->selection_com_port->setStyleSheet(
                    "QComboBox {"
                    "   color: #555555;"
                    "    border: 1px solid gray;"
                    "    border-radius: 3px;"
                    "    min-width: 6em;"
                    "}"
                    ""
                    "QComboBox:editable {"
                    "    background: white;"
                    "}"
                    ""
                    "QComboBox:!editable, QComboBox::drop-down:editable {"
                    "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                    "                                 stop: 0 white, stop: 0.4 white,"
                    "                                 stop: 1.0 #F3F3F3);"
                    "}"
                    ""

                    "QComboBox:!editable:on, QComboBox::drop-down:editable:on {"
                    "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                    "                                stop: 0 #F3F3F3, stop: 0.6 white,"
                    "                                stop: 1.0 white);"
                    "}"
                    ""
                    "QComboBox:on {" /* shift the text when the popup opens */
                    "    padding-top: 3px;"
                    "    padding-left: 4px;"
                    "}"
                    ""
                    "QComboBox::drop-down {"
                    "    subcontrol-origin: padding;"
                    "    subcontrol-position: top right;"
                    "    width: 15px;"
                    ""
                    "    border-left-width: 1px;"
                    "    border-left-color: darkgray;"
                    "    border-left-style: solid;" /* just a single line */
                    "    border-top-right-radius: 3px;" /* same radius as the QComboBox */
                    "    border-bottom-right-radius: 3px;"
                    "}"
                    ""
//                    "QComboBox::down-arrow {"
//                    "    image: url(:/resource/icons/16x16/1downarrow.png);"
//                    "}"
//                    ""
                    "QComboBox::down-arrow:on {" /* shift the arrow when popup is open */
                    "    top: 1px;"
                    "    left: 1px;"
                    "}");
    ui->info_com_port->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_2->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_3->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_4->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_5->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_6->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_7->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_8->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_shakes_number->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_current->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_strength->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_shake_time->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_cool_time->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_stop_current->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_stop_strenghth->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_16->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_17->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_18->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_19->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_20->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start_2->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start_3->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start_4->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start_5->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start->setStyleSheet(
                "QLabel{"
                "   color: #555555;"
                "}"
                );
    ui->info_pause->setStyleSheet(
                "QLabel{"
                "   color: #555555;"
                "}"
                );
    ui->info_stop->setStyleSheet(
                "QLabel{"
                "   color: #555555;"
                "}"
                );
    ui->scrollArea->setObjectName( "scrollArea1" );
    ui->scrollArea->setStyleSheet(
                    "#scrollArea1{"
                    "   color: #555555;"
                    "   background-color: white;"
                    "   border-radius: 3px;"
                    "border: 1px solid #d5d5d5;"
                    "}"
                    );
    ui->frame->setObjectName( "frame1" );
    ui->frame->setStyleSheet(
                "#frame1{"
                "border-radius: 3px;;"
                "border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_2->setObjectName( "frame2" );
    ui->frame_2->setStyleSheet(
                "#frame2{"
                "border-radius: 3px;"
                "border-bottom-left-radius: 10px;"
                "border-bottom-right-radius: 10px;"
                "border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_3->setObjectName( "frame3" );
    ui->frame_3->setStyleSheet(
                "#frame3{"
                "border-radius: 3px;"
                "border: 1px solid #d5d5d5;"
                "}"
                );
}

QElapsedTimer timerUpdate;

void MainWindow::update_ui()
{
    if(sendFlag)
    {
        serial->write("UPDATE");
        qDebug() << "UPDATE"<< timerUpdate.elapsed();
        QTimer::singleShot(PROSITY, this, SLOT(update_ui()));
        timerUpdate.start();
    }
    ui->receive_shakes_number->setText(QString::number(receiveVector[0]));
    ui->receive_current->setText(QString::number(receiveVector[1]));
    ui->receive_strength->setText(QString::number(receiveVector[2]));
    ui->receive_shake_time->setText(QString::number(receiveVector[3]));
    ui->receive_cool_time->setText(QString::number(receiveVector[4]));
    ui->receive_stop_current->setText(QString::number(receiveVector[5]));
    ui->receive_stop_strenghth->setText(QString::number(receiveVector[6]));
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

    if (    ((byteArrayMessage >= "0") && (byteArrayMessage <= "9")) || (byteArrayMessage == "a") || (byteArrayMessage == "b") ||
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


}

QElapsedTimer timer;
bool flagFirst = false;
bool flagSecond = false;
bool flagThird = true;
void MainWindow::on_send_shake_time_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = SHAKE_TIME;
            byteArraySendMessage[2] = arg1.toInt()>>8;
            byteArraySendMessage[3] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_send_cool_time_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = COOL_TIME;
            byteArraySendMessage[2] = arg1.toInt()>>8;
            byteArraySendMessage[3] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_send_stop_strength_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = STOP_STRENGHT;
            byteArraySendMessage[2] = arg1.toInt()>>8;
            byteArraySendMessage[3] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_send_stop_current_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = STOP_CURRENT;
            byteArraySendMessage[2] = arg1.toInt()>>8;
            byteArraySendMessage[3] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_start_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = MOVEMENT;
            byteArraySendMessage[2] = 1;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_pause_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = MOVEMENT;
            byteArraySendMessage[2] = 2;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_stop_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = MOVEMENT;
            byteArraySendMessage[2] = 3;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::on_tenso_calib_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = TENSO_CALIB;
            byteArraySendMessage[1] = 0;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}

void MainWindow::buffer_send_message ()
{
    if (timer.elapsed()>=PROSITY)
    {
        flagSecond = true;
    }
    if(flagSecond && flagFirst)
    {
        sendFlag = false;
        flagFirst = false;
        flagSecond = false;
        flagThird = false;
        if(PROSITY-timerUpdate.elapsed()+20>=0)
        {
            QTimer::singleShot(PROSITY-timerUpdate.elapsed()+20, this, SLOT(send_message()));
        }else
        {
            QTimer::singleShot(0, this, SLOT(send_message()));
        }
    }
}

void MainWindow::send_message ()
{
    qDebug() << "MESSAGE"<<timerUpdate.elapsed();
    timerUpdate.start();
    sendFlag = true;
    flagThird = true;
    int temp = byteArraySendMessage[1];
    int temp2 = byteArraySendMessage[0];
    QTimer::singleShot(PROSITY, this, SLOT(update_ui()));
    if(temp == 1)
    {
        QByteArray arrey;
        arrey[0] = byteArraySendMessage[0];
        arrey[1] = byteArraySendMessage[1];
        arrey[2] = byteArraySendMessage[2];
        serial->write(arrey);

        QString oldLable = ui->label->text();
        ui->label->setText(oldLable +
                           "---> "+arrey.toHex() +"\n");
    } else
    {
        if(temp2 == 2)
        {
            serial->write("TENSO_CALIB");

            QString oldLable = ui->label->text();
            ui->label->setText(oldLable +
                               "---> "+"TENSO_CALIB" +"\n");
        }else
        {
            serial->write(byteArraySendMessage);

            QString oldLable = ui->label->text();
            ui->label->setText(oldLable +
                               "---> "+byteArraySendMessage.toHex() +"\n");
        }
    }
}


int sendByte = 0;
void MainWindow::sendBytes (QString arg1)
{
    QStringList listBytes = arg1.split(" ");
    if(listBytes[listBytes.size()-1].size() == 1){ui->edit_line->setText(ui->edit_line->text() + "0");} else {  }
    listBytes = ui->edit_line->text().split(" ");
    QString editedArg = arg1;
    QByteArray byteArrayMessage;


    for (int i=0; i<=listBytes.size()-1; i++)
    {

        QString firstChar = "0";
        QString secondChar = "0";
        firstChar = listBytes[i][0];
        secondChar = listBytes[i][1];
        if(firstChar == "0") {sendByte = 0 * 16; separateSecondByte(secondChar);}
        if(firstChar == "1") {sendByte = 1 * 16; separateSecondByte(secondChar);}
        if(firstChar == "2") {sendByte = 2 * 16; separateSecondByte(secondChar);}
        if(firstChar == "3") {sendByte = 3 * 16; separateSecondByte(secondChar);}
        if(firstChar == "4") {sendByte = 4 * 16; separateSecondByte(secondChar);}
        if(firstChar == "5") {sendByte = 5 * 16; separateSecondByte(secondChar);}
        if(firstChar == "6") {sendByte = 6 * 16; separateSecondByte(secondChar);}
        if(firstChar == "7") {sendByte = 7 * 16; separateSecondByte(secondChar);}
        if(firstChar == "8") {sendByte = 8 * 16; separateSecondByte(secondChar);}
        if(firstChar == "9") {sendByte = 9 * 16; separateSecondByte(secondChar);}
        if(firstChar == "a") {sendByte = 10 * 16; separateSecondByte(secondChar);}
        if(firstChar == "b") {sendByte = 11 * 16; separateSecondByte(secondChar);}
        if(firstChar == "c") {sendByte = 12 * 16; separateSecondByte(secondChar);}
        if(firstChar == "d") {sendByte = 13 * 16; separateSecondByte(secondChar);}
        if(firstChar == "e") {sendByte = 14 * 16; separateSecondByte(secondChar);}
        if(firstChar == "f") {sendByte = 15 * 16; separateSecondByte(secondChar);}
    }

    QString oldLable = ui->label->text();
    ui->label->setText(oldLable +
                       "---> "+formatedEditLine +"\n");//QString::number(byteArrayMessage.count())
}

void MainWindow::separateSecondByte (QString secondByte)
{
    QByteArray byteArrayMessage;

    if(secondByte == "0")
    {
        sendByte += 0;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "1")
    {
        sendByte += 1;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "2")
    {
        sendByte += 2;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "3")
    {
        sendByte += 3;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "4")
    {
        sendByte += 4;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "5")
    {
        sendByte += 5;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "6")
    {
        sendByte += 6;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "7")
    {
        sendByte += 7;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "8")
    {
        sendByte += 8;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "9")
    {
        sendByte += 9;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "a")
    {
        sendByte += 10;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "b")
    {
        sendByte += 11;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "c")
    {
        sendByte += 12;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "d")
    {
        sendByte += 13;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "e")
    {
        sendByte += 14;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
    if(secondByte == "f")
    {
        sendByte += 15;
        byteArrayMessage[0] = sendByte;
        serial->write(byteArrayMessage);
    }
}

void MainWindow::on_connect_button_clicked()
{
    int index =  ui->selection_com_port->currentIndex();

    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName(ui->selection_com_port->currentText());//указали com-порт и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open((QIODevice::ReadWrite));//открыли порт

    connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecieve()));//соединяем чтение-приём данных

    qDebug()<<"Выбрали: "+ui->selection_com_port->currentText()+"Определился: " + serial->portName();

    QString oldLable = ui->label->text();
    ui->label->setText(QString::number(index) + " " + ui->selection_com_port->currentText()+ "\n");
}



