#include "ui_mainwindow.h"
#include "LogCategories.h"
#include "constant.h"
#include "mainwindow.h"


int receiveCurrent = 0;
int receiveStrength = 0;
int receiveStopCurrent = 0;
int receiveCoolTime = 0;
int receiveShakeTime = 0;
int receiveStopStrenghth = 0;
int receiveShakesNumber = 0;

QFile fileLog("Log/Log__"+QDateTime::currentDateTime().toString("hh-mm-ss  dd.MM.yyyy") + ".txt");
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_stule();
    serialBuffer = "";
    productIdentifier = "";
    vendorIdentifier = "";
    QDir().mkdir("Log");

//    ui->edit_line->setText("f0 ab 1d");

    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName("COM5");//указали com-порт
    serial->open((QIODevice::ReadWrite));//открыли и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    update_ui();//вызов повторяющейся функции обновления отображаемых параметров
    add_graph();//инициализация графиков в подготовленную форму

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

bool MainWindow::m_synchronized = false;


QElapsedTimer timerGrip;
quint16 receiveVector[10];
QString reseiveMessage = "";
bool flagIdlingCompression = true; //правдив пока рука сжимается на холостом ходу
bool flagStartTimerGrip = true;
int numberOfIdleCurrentValues = 0;
int sumCurrent = 0;
int numberNoiseLevel = 0;
int sumNoiseLevel = 0;
int numberVoltage = 0;
int sumVoltage = 0;
int timeOfIdleGrip = 0;
//int schetchik = 0;
//int schetchik2 = 2;
//bool first = true;
//bool second = true;
int lineLength = 0;
QList<QByteArray> sep;
QString packet = "";
void MainWindow::serialRecieve()//получаем данные
{
    if(VANIA_MATVEI){
        //    schetchik++;
        //    if(schetchik % 255 == 1){second = true;}
            byteArreyReceiveMessage = serial->readAll();//читаем всё
//            qDebug()<< "Строка "<<packet;
        //    byteArreyReceiveMessage[0] = 0;
        //    if(first)
        //    {
        //        byteArreyReceiveMessage[1] = 1;
        //        first=false;
        //    } else {
        //        if(second)
        //        {
        //            byteArreyReceiveMessage[1] = schetchik2++;
        //            second=false;
        //        } else {
        //            byteArreyReceiveMessage[1] = schetchik2;
        //        }
        //    }
        //    byteArreyReceiveMessage[2] = 0;
        //    byteArreyReceiveMessage[3] = 2+schetchik;
        //    byteArreyReceiveMessage[4] = 0;
        //    byteArreyReceiveMessage[5] = 3+schetchik;
        //    byteArreyReceiveMessage[6] = 0;
        //    byteArreyReceiveMessage[7] = 4+schetchik;
        //    byteArreyReceiveMessage[8] = 0;
        //    byteArreyReceiveMessage[9] = 5+schetchik;
        //    byteArreyReceiveMessage[10] = 0;
        //    byteArreyReceiveMessage[11] = 6+schetchik;
        //    byteArreyReceiveMessage[12] = 0;
        //    byteArreyReceiveMessage[13] = 7+schetchik;
        //    byteArreyReceiveMessage[14] = 8+schetchik;
        //    byteArreyReceiveMessage[15] = 9+schetchik;
        //    byteArreyReceiveMessage[16] = 10+schetchik;
        //    byteArreyReceiveMessage[17] = 11+schetchik;
        //    byteArreyReceiveMessage[18] = 28;
        //    byteArreyReceiveMessage[19] = 134-schetchik;
            QDataStream dataStream(byteArreyReceiveMessage);
            serialBuffer = byteArreyReceiveMessage.toHex();

            for(int i=0; i<byteArreyReceiveMessage.size(); i+=2)
            {
                receiveVector[i/2] =qFromBigEndian<quint16>(((const uchar*)byteArreyReceiveMessage.constData()+i));
            }
    }else{
        for(;;)
            {
                const auto bytesAvailable = serial->bytesAvailable();
                if(!m_synchronized) {
                    if(bytesAvailable < 4)
                        return;
                    const auto head = serial->peek(4);
                    if(head != "FCD>") { //в head должна быть запись начала пакета. В моём случае начало пакета это FCD> у тебя будет другое
                        serial->read(1);
                        continue;
                    } else {
                        m_synchronized = true;
                    }
                } else {
                    const auto probe = serial->peek(bytesAvailable);
                    const auto tailIndex = probe.indexOf("\r");
                    if(tailIndex == -1)
                        return;
                    packet = serial->read(tailIndex + 2);
                    qDebug() << "Ответ записи значений: " << packet;
                    QList<QString> value = packet.split(' ');

                    for(int i=0; i<14; i+=2)
                    {
                        receiveVector[i/2] =value[i/2+1].toInt();
                    }
                    quint16 temp = 0;
                    temp = value[8].toInt() << 8;
                    receiveVector[7] = temp + value[9].toInt();
                    temp = value[10].toInt() << 8;
                    receiveVector[8] = temp + value[11].toInt();
                    receiveVector[9] = value[12].toInt();
                    qDebug() << receiveVector[0] << receiveVector[1] << receiveVector[2] << receiveVector[3] << receiveVector[4] << receiveVector[5] << receiveVector[6] << receiveVector[7] << receiveVector[8];
                    m_synchronized = false;
                }
            }
    }
    if(flagIdlingCompression)
    {
//        qDebug()<< "flagIdlingCompression = true";
        sumCurrent += receiveCurrent;
        numberOfIdleCurrentValues++;
        meanCurrent = sumCurrent/numberOfIdleCurrentValues;
        ui->receive_current_max->setText(QString::number(meanCurrent));
        if(flagStartTimerGrip)
        {
            timerGrip.start();
            flagStartTimerGrip = false;
        }
        timeOfIdleGrip = timerGrip.elapsed();
    }

    //calculate mean noise level
    sumNoiseLevel += receiveVector[8]&255;
    numberNoiseLevel ++;
    meanNoiseLevel = sumNoiseLevel/numberNoiseLevel;

    //calculate mean voltage level
    sumVoltage += receiveVector[9];
    numberVoltage ++;
    meanVoltage = sumVoltage/numberVoltage;
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
    ui->send_nominal_temperature->setStyleSheet(
                     "QLineEdit{"
                     "   color: #555555;"
                     "   background-color: white;"
                     "   border: lpx solid #828282;"
                     "   border-radius: 3px"
                     "}");
    ui->send_stop_temperature->setStyleSheet(
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
    ui->start->setStyleSheet(
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
    ui->pause->setStyleSheet(
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
    ui->stop->setStyleSheet(
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
    ui->open->setStyleSheet(
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
    ui->close->setStyleSheet(
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
    ui->mio_set_0->setStyleSheet(
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
    ui->contorl_mode->setStyleSheet(
                    "QRadioButton{"
                    "   color: #555555;"
                    "}"
                    ""
                    "QRadioButton::indicator {"
                    "   width:                  10px;"
                    "   height:                 10px;"
                    "   border-radius:          7px;"
                    "}"
                    ""
                    "QRadioButton::indicator:checked {"
                    "    background-color:       #555555;"
                    "    border:                 2px solid #828282;"
                    "}"
                    ""
                    "QRadioButton::indicator:unchecked {"
                    "    background-color:       white;"
                    "    border:                 2px solid #828282;"
                    "}");
    ui->contorl_mode_2->setStyleSheet(
                    "QRadioButton{"
                    "   color: #555555;"
                    "}"
                    ""
                    "QRadioButton::indicator {"
                    "   width:                  10px;"
                    "   height:                 10px;"
                    "   border-radius:          7px;"
                    "}"
                    ""
                    "QRadioButton::indicator:checked {"
                    "    background-color:       #555555;"
                    "    border:                 2px solid #828282;"
                    "}"
                    ""
                    "QRadioButton::indicator:unchecked {"
                    "    background-color:       white;"
                    "    border:                 2px solid #828282;"
                    "}");
    ui->contorl_mode_3->setStyleSheet(
                    "QRadioButton{"
                    "   color: #555555;"
                    "}"
                    ""
                    "QRadioButton::indicator {"
                    "   width:                  10px;"
                    "   height:                 10px;"
                    "   border-radius:          7px;"
                    "}"
                    ""
                    "QRadioButton::indicator:checked {"
                    "    background-color:       #555555;"
                    "    border:                 2px solid #828282;"
                    "}"
                    ""
                    "QRadioButton::indicator:unchecked {"
                    "    background-color:       white;"
                    "    border:                 2px solid #828282;"
                    "}");
    ui->contorl_mode_4->setStyleSheet(
                    "QRadioButton{"
                    "   color: #555555;"
                    "}"
                    ""
                    "QRadioButton::indicator {"
                    "   width:                  10px;"
                    "   height:                 10px;"
                    "   border-radius:          7px;"
                    "}"
                    ""
                    "QRadioButton::indicator:checked {"
                    "    background-color:       #555555;"
                    "    border:                 2px solid #828282;"
                    "}"
                    ""
                    "QRadioButton::indicator:unchecked {"
                    "    background-color:       white;"
                    "    border:                 2px solid #828282;"
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
                    "QComboBox::down-arrow {"
                    "    image: url(:/resource/icons/16x16/down-arrow3.png);"
                    "}"
                    ""
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
    ui->info_start_6->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_start_7->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_current_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_strength_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_current_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_strength_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_9->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_10->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_temperature->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_noise_level->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_21->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_22->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_2->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_3->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_4->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_5->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_6->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_7->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_8->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_9->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_10->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->label_invert_11->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_noise_level_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_noise_level_max_2->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_noise_level_max->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_voltage_mean->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_11->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_12->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_13->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_14->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_15->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_23->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_24->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_25->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_26->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_27->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->info_com_port_28->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->max_energy->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->max_energy_calculated->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->energy_rest->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->energy_rest_calculated->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->enargy_shake->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_nominal_temperature->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_stop_temperature->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->receive_voltage->setStyleSheet(
                    "QLabel{"
                    "   color: #555555;"
                    "}"
                    );
    ui->frame->setObjectName( "frame1" );
    ui->frame->setStyleSheet(
                "#frame1{"
                "   border-radius: 3px;"
                "   border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_2->setObjectName( "frame2" );
    ui->frame_2->setStyleSheet(
                "#frame2{"
                "   border-radius: 3px;"
                "   border-bottom-left-radius: 10px;"
                "   border-bottom-right-radius: 10px;"
                "   border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_3->setObjectName( "frame3" );
    ui->frame_3->setStyleSheet(
                "#frame3{"
                "   border-radius: 3px;"
                "   border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_4->setObjectName( "frame4" );
    ui->frame_4->setStyleSheet(
                "#frame4{"
                "   border-radius: 3px;"
                "   border-bottom-left-radius: 10px;"
                "   border-bottom-right-radius: 10px;"
                "   border: 1px solid #d5d5d5;"
                "}"
                );
    ui->frame_5->setObjectName( "frame5" );
    ui->frame_5->setStyleSheet(
                "#frame5{"
                "   border-radius: 3px;"
                "   border: 1px solid #d5d5d5;"
                "}"
                );
    ui->servo_angle->setStyleSheet(
                "QSlider::groove:horizontal{"
                "   border-radius: 3px;"
                "   height: 8px;"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d5d5d5, stop:1 #d5d5d5);"
                "   margin: 2px 0;"
                "}"
                ""
                "QSlider::handle:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #828282, stop:1 #828282);"
                "   width: 18px;"
                "   margin: -2px 0;"
                "   border-radius: 3px;"
                "}"
                );
    ui->start_servo_angle->setStyleSheet(
                "QSlider::groove:horizontal{"
                "   border-radius: 3px;"
                "   height: 8px;"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d5d5d5, stop:1 #d5d5d5);"
                "   margin: 2px 0;"
                "}"
                ""
                "QSlider::handle:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #828282, stop:1 #828282);"
                "   width: 18px;"
                "   margin: -2px 0;"
                "   border-radius: 3px;"
                "}"
                );
    ui->stop_servo_angle->setStyleSheet(
                "QSlider::groove:horizontal{"
                "   border-radius: 3px;"
                "   height: 8px;"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d5d5d5, stop:1 #d5d5d5);"
                "   margin: 2px 0;"
                "}"
                ""
                "QSlider::handle:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #828282, stop:1 #828282);"
                "   width: 18px;"
                "   margin: -2px 0;"
                "   border-radius: 3px;"
                "}"
                );
}

QElapsedTimer timerUpdate;
//bool flagIdlingCompression = false;
bool flagFirstShake = true;
int oldShakeNumber = 0;
void MainWindow::update_ui()
{
    if(sendFlag)
    {
        QByteArray byteArrayMessage;
        byteArrayMessage[0] = 0;
//        serial->write(byteArrayMessage);
        QTimer::singleShot(PROSITY, this, SLOT(update_ui()));
        timerUpdate.start();
    }
    if(!mCyclogramMode){
        receiveShakesNumber = (65535 * cycleMultiplier)+receiveVector[0];
    }
    receiveCurrent = receiveVector[1];
    receiveStrength = (receiveVector[2]-658)*2;
    receiveShakeTime = receiveVector[3];
    receiveCoolTime = receiveVector[4];
    receiveStopCurrent = receiveVector[5];
    receiveStopStrenghth = receiveVector[6];
    ui->receive_shakes_number->setText(QString::number(receiveShakesNumber));
    ui->receive_current->setText(QString::number(receiveCurrent));
    ui->receive_strength->setText(QString::number(receiveStrength));
    ui->receive_shake_time->setText(QString::number(receiveShakeTime));
    ui->receive_cool_time->setText(QString::number(receiveCoolTime));
    ui->receive_stop_current->setText(QString::number(receiveStopCurrent));
    ui->receive_stop_strenghth->setText(QString::number(receiveStopStrenghth));
    ui->receive_temperature->setText(QString::number(receiveVector[7]>>8));
    ui->receive_nominal_temperature->setText(QString::number(receiveVector[7]&255));
    ui->receive_stop_temperature->setText(QString::number(receiveVector[8]>>8));
    ui->receive_noise_level->setText(QString::number(receiveVector[8]&255));
    ui->receive_voltage->setText(QString::number(receiveVector[9]));

    if(((oldShakeNumber < receiveShakesNumber) && (oldShakeNumber != 0)) || (oldShakeNumber == 65535))
    {
        writeToFileLog();
        ui->receive_strength_max->setText(QString::number(printStrenght));
        ui->receive_noise_level_max->setText(QString::number(meanNoiseLevel));
        ui->receive_voltage_mean->setText(QString::number(meanVoltage));
        printStrenght = 0;
        printTemperature = 0;
        flagIdlingCompression = true;
//        qDebug()<< "real flagIdlingCompression = true";
        meanCurrent = 0;
        numberOfIdleCurrentValues = 0;
        sumCurrent = 0;
        meanNoiseLevel = 0;
        sumNoiseLevel = 0;
        numberNoiseLevel = 0;
        if(flagFirstShake)
        {
            startVoltage = meanVoltage;
            maxButteryEnergyCalculated = ((startVoltage/1000*1.2*3600)-MIN_BATTERY_ENERGY_TEORETICAL)*3.3636;
//            qDebug() << "startVoltage = " + QString::number(startVoltage/1000) +  "   maxButteryEnergyCalculated = " + QString::number(maxButteryEnergyCalculated);
            ui->max_energy->setText(QString::number(MAX_BATTERY_ENERGY_TEORETICAL));
            ui->max_energy_calculated->setText(QString::number(maxButteryEnergyCalculated));
            lastMeanCycleVoltage = meanVoltage;
            flagFirstShake = false;
        }
        periodEnergy = (lastMeanCycleVoltage - meanVoltage)/1000*1.2*3600;

        sumPeriodEnergy += periodEnergy;
        enrgyRestPractical = maxButteryEnergyCalculated - sumPeriodEnergy;
        enrgyRestTheoretical = MAX_BATTERY_ENERGY_TEORETICAL - sumPeriodEnergy;
        ui->energy_rest->setText(QString::number(enrgyRestTheoretical));
        ui->energy_rest_calculated->setText(QString::number(enrgyRestPractical));
        ui->enargy_shake->setText(QString::number(periodEnergy));
        lastMeanCycleVoltage = meanVoltage;
        meanVoltage = 0;
        sumVoltage = 0;
        numberVoltage = 0;
        flagStartTimerGrip = true;
        timeOfIdleGrip = 0;
    }
    if(printStrenght < (receiveStrength)){printStrenght = receiveStrength;}
    if(printTemperature < (receiveVector[7]>>8)){printTemperature = receiveVector[7]>>8;}
    if(printStrenght >= MIN_CICLOGRAMM_STRENGHT){
//        qDebug()<< "real flagIdlingCompression = false";
        flagIdlingCompression = false;
    }

    oldShakeNumber = receiveShakesNumber;
    if(receiveShakesNumber == 65535)
    {
        cycleMultiplier++;
        receiveShakesNumber = 1;
    }
    if(mCyclogramStart){
        generationCyclogram();
    }
}

QString formatedEditLine = "";
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
void MainWindow::on_send_nominal_temperature_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = NOMINAL_TEMPERATURE;
            byteArraySendMessage[2] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_send_stop_temperature_textChanged(const QString &arg1)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = STOP_TEMPERATURE;
            byteArraySendMessage[2] = arg1.toInt();
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_start_clicked()
{
    if(mCyclogramMode){
        mCyclogramStart = true;
    } else {
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
}
void MainWindow::on_pause_clicked()
{
    mCyclogramStart = false;
    resetCyclogram();
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
    mCyclogramStart = false;
    resetCyclogram();
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
void MainWindow::on_open_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = SINGLE_MOVEMENT;
            byteArraySendMessage[2] = OPEN;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }

}
void MainWindow::on_close_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = SINGLE_MOVEMENT;
            byteArraySendMessage[2] = CLOSE;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_mio_set_0_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = MIO_SET_0;
            byteArraySendMessage[1] = 0;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_invert_clicked()
{
    if(ui->invert->isChecked())
    {
        if(flagThird)
        {
            timer.start();
            flagFirst = true;
            flagSecond = false;
            for (int i=PROSITY-10; i<=PROSITY+20; i++)
            {
                byteArraySendMessage[0] = SEND;
                byteArraySendMessage[1] = SINGLE_MOVEMENT;
                byteArraySendMessage[2] = INVERT;
                QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            }
        }
    } else
    {
        if(flagThird)
        {
            timer.start();
            flagFirst = true;
            flagSecond = false;
            for (int i=PROSITY-10; i<=PROSITY+20; i++)
            {
                byteArraySendMessage[0] = SEND;
                byteArraySendMessage[1] = SINGLE_MOVEMENT;
                byteArraySendMessage[2] = CANCEL_INVERT;
                QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            }
        }
    }
}
void MainWindow::on_test_HDLC_clicked()
{
    if(ui->test_HDLC->isChecked())
    {
        if(flagThird)
        {
            timer.start();
            flagFirst = true;
            flagSecond = false;
            for (int i=PROSITY-10; i<=PROSITY+20; i++)
            {
                byteArraySendMessage[0] = SEND;
                byteArraySendMessage[1] = SINGLE_MOVEMENT;
                byteArraySendMessage[2] = TEST_HDLC_ON;
                QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            }
        }
    } else
    {
        if(flagThird)
        {
            timer.start();
            flagFirst = true;
            flagSecond = false;
            for (int i=PROSITY-10; i<=PROSITY+20; i++)
            {
                byteArraySendMessage[0] = SEND;
                byteArraySendMessage[1] = SINGLE_MOVEMENT;
                byteArraySendMessage[2] = TEST_HDLC_OFF;
                QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            }
        }
    }
}
void MainWindow::on_contorl_mode_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = CONTROL_MODE;
            byteArraySendMessage[2] = HDLC;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            mCyclogramMode = false;
            mCyclogramStart = false;
        }
    }
}
void MainWindow::on_contorl_mode_2_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = CONTROL_MODE;
            byteArraySendMessage[2] = SERVO;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            mCyclogramMode = false;
            mCyclogramStart = false;
        }
    }
}
void MainWindow::on_contorl_mode_3_clicked()
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = CONTROL_MODE;
            byteArraySendMessage[2] = DC;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
            mCyclogramMode = false;
            mCyclogramStart = false;
        }
    }
}
void MainWindow::on_contorl_mode_4_clicked()
{
    mCyclogramMode = true;
}
void MainWindow::on_servo_angle_valueChanged(int value)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = SERVO_POSITION;
            byteArraySendMessage[2] = value;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_start_servo_angle_valueChanged(int value)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = START_SERVO_ANGLE;
            byteArraySendMessage[2] = value;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_stop_servo_angle_valueChanged(int value)
{
    if(flagThird)
    {
        timer.start();
        flagFirst = true;
        flagSecond = false;
        for (int i=PROSITY-10; i<=PROSITY+20; i++)
        {
            byteArraySendMessage[0] = SEND;
            byteArraySendMessage[1] = STOP_SERVO_ANGLE;
            byteArraySendMessage[2] = value;
            QTimer::singleShot(i, this, SLOT(buffer_send_message()));
        }
    }
}
void MainWindow::on_connect_button_clicked()
{
    serial = new QSerialPort(this);//новый экземпляр класса AbstractSerial
    serial->setPortName(ui->selection_com_port->currentText());//указали com-порт и параметры порта (далее)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open((QIODevice::ReadWrite));//открыли порт

    connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecieve()));//соединяем чтение-приём данных
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
//    qDebug() << "MESSAGE"<<timerUpdate.elapsed();
    timerUpdate.start();
    sendFlag = true;
    flagThird = true;
    int temp = byteArraySendMessage[1];
    int temp2 = byteArraySendMessage[0];
    QTimer::singleShot(PROSITY, this, SLOT(update_ui()));
    if(temp == MOVEMENT || temp == SINGLE_MOVEMENT || temp == CONTROL_MODE || temp == SERVO_POSITION ||
        temp == START_SERVO_ANGLE || temp == STOP_SERVO_ANGLE || temp == NOMINAL_TEMPERATURE ||
        temp == STOP_TEMPERATURE)
    {   
        QByteArray arrey;
        arrey[0] = byteArraySendMessage[0];
        arrey[1] = byteArraySendMessage[1];
        arrey[2] = byteArraySendMessage[2];
        serial->write(arrey);
    } else
    {
        if(temp == SHAKE_TIME || temp == COOL_TIME || temp == STOP_CURRENT || temp == STOP_STRENGHT)
        {
            QByteArray arrey;
            arrey[0] = byteArraySendMessage[0];
            arrey[1] = byteArraySendMessage[1];
            arrey[2] = byteArraySendMessage[2];
            arrey[3] = byteArraySendMessage[3];
            serial->write(arrey);
        } else {
            if(temp2 == 2)
            {
                serial->write("TENSO_CALIB");
            }else
            {
                if(temp2 == 3)
                {
                    serial->write("MIO_SET_0");
                }else
                {
                    serial->write(byteArraySendMessage);
                }
            }
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

/****************************************************************
 * Function Name : add_graph
 * Description   : Displays the real time graph on the GUI
 * Returns       : None
 * Params        : None
 ****************************************************************/
void MainWindow::add_graph()
{

    /////////////////////////////////////////////////////
    /// инициализация графика силы тока
    /////////////////////////////////////////////////////
    /* Add graph and set the curve line color to green */
    ui->current_plot->addGraph();
    ui->current_plot->graph(0)->setPen(QPen(Qt::darkRed));
    ui->current_plot->graph(0)->setAntialiased(true);
    ui->current_plot->graph(0)->setAntialiasedFill(true);

    /* Configure x-Axis as time in secs */
    QSharedPointer<QCPAxisTickerTime> timeTickerStrenghtGraph(new QCPAxisTickerTime);
    timeTickerStrenghtGraph->setTimeFormat("%s");
    ui->current_plot->xAxis->setTicker(timeTickerStrenghtGraph);
    ui->current_plot->axisRect()->setupFullAxesBox();

    /* Configure x and y-Axis to display Labels */
    ui->current_plot->xAxis->setTickLabelFont(QFont(QFont().family(),6));
    ui->current_plot->yAxis->setTickLabelFont(QFont(QFont().family(),6));
    ui->current_plot->yAxis->setLabel("мА");
//    ui->current_plot->xAxis->setLabel("Время (с)");

    /* Make top and right axis visible, but without ticks and label */
    ui->current_plot->xAxis2->setVisible(true);
    ui->current_plot->yAxis->setVisible(true);
    ui->current_plot->xAxis2->setTicks(false);//false
    ui->current_plot->yAxis2->setTicks(false);//false
    ui->current_plot->xAxis2->setTickLabels(false);//false
    ui->current_plot->yAxis2->setTickLabels(false);//false

    /////////////////////////////////////////////////////
    /// инициализация графика тензодатчика
    /////////////////////////////////////////////////////
    /* Add graph and set the curve line color to green */
    ui->strenght_plot->addGraph();
    ui->strenght_plot->graph(0)->setPen(QPen(Qt::darkRed));
    ui->strenght_plot->graph(0)->setAntialiased(true);
    ui->strenght_plot->graph(0)->setAntialiasedFill(true);

    /* Configure x-Axis as time in secs */
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    ui->strenght_plot->xAxis->setTicker(timeTicker);
    ui->strenght_plot->axisRect()->setupFullAxesBox();

    /* Configure x and y-Axis to display Labels */
    ui->strenght_plot->xAxis->setTickLabelFont(QFont(QFont().family(),6));
    ui->strenght_plot->yAxis->setTickLabelFont(QFont(QFont().family(),6));
    ui->strenght_plot->yAxis->setLabel("г");
    ui->strenght_plot->xAxis->setLabel("Время (с)");

    /* Make top and right axis visible, but without ticks and label */
    ui->strenght_plot->xAxis2->setVisible(true);
    ui->strenght_plot->yAxis->setVisible(true);
    ui->strenght_plot->xAxis2->setTicks(false);//false
    ui->strenght_plot->yAxis2->setTicks(false);//false
    ui->strenght_plot->xAxis2->setTickLabels(false);//false
    ui->strenght_plot->yAxis2->setTickLabels(false);//false

    /* Set up and initialize the graph plotting timer */
    connect(&timer_plot, SIGNAL(timeout()),this,SLOT(realtimePlot()));
    timer_plot.start(5);
}

/****************************************************************
 * Function Name : realtimePlot
 * Description   : Displays the real time graph on the GUI
 * Returns       : None
 * Params        : None
 ****************************************************************/
void MainWindow::realtimePlot()
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;//100
    static double lastPointKey = 0;
    if(key - lastPointKey > 0.033)//0.002
    {
        ui->strenght_plot->graph(0)->addData(key, receiveStrength);
        ui->current_plot->graph(0)->addData(key, receiveCurrent);
        lastPointKey = key;
    }

    /* make key axis range scroll right with the data at a constant range of 8. */
    ui->strenght_plot->graph(0)->rescaleValueAxis();
    ui->current_plot->graph(0)->rescaleValueAxis();
    ui->strenght_plot->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->current_plot->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->strenght_plot->replot();
    ui->current_plot->replot();
}

/****************************************************************
 * Function Name : generationCyclogram
 * Description   : Generation of the cyclogram of mooving artificial limb
 * Returns       : None
 * Params        : None
 ****************************************************************/
QElapsedTimer timeGenerationCyclogram;
bool stateChange = true;
int currentIterator = 0;
void MainWindow::generationCyclogram() {
    if(cyclogramStation == FIRST_STATE){
        if(stateChange){
            if(flagThird)
            {
                timer.start();
                flagFirst = true;
                flagSecond = false;
                for (int i=PROSITY-10; i<=PROSITY+20; i++)
                {
                    byteArraySendMessage[0] = SEND;
                    byteArraySendMessage[1] = SINGLE_MOVEMENT;
                    byteArraySendMessage[2] = OPEN;
                    QTimer::singleShot(i, this, SLOT(buffer_send_message()));

                }
            } // посылка команды на открытие
            stateChange = false;
            timeGenerationCyclogram.start();
            qDebug() << "CyclogramStation = FIRST_STATE";
        } else {
            //дописать логику считывания нескольких показаний тока
            if(receiveCurrent >= receiveStopCurrent){
               if(currentIterator==NUMBER_CURRENT_ITERATION_FOR_CUT_OFF ){
                   cyclogramStation = SECOND_STATE;
                   stateChange = true;
                   currentIterator = 0;
               }
                currentIterator++;
            }
            if(timeGenerationCyclogram.elapsed() >= MAX_CICLOGRAMM_STAGE_TIME){
                cyclogramStation = SECOND_STATE;
                stateChange = true;
                currentIterator = 0;
            }
        }
    }
    if(cyclogramStation == SECOND_STATE){
        if(stateChange){
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
            }// посылка команды на остановку
            stateChange = false;
            timeGenerationCyclogram.start(); 
            qDebug() << "CyclogramStation = SECOND_STATE";
        } else {
            if(timeGenerationCyclogram.elapsed() >= receiveCoolTime || timeGenerationCyclogram.elapsed() >= MAX_CICLOGRAMM_STAGE_TIME){
                cyclogramStation = THRID_STATE;
                stateChange = true;
            }
        }
    }
    if(cyclogramStation == THRID_STATE){
        if(stateChange){
            if(flagThird)
            {
                timer.start();
                flagFirst = true;
                flagSecond = false;
                for (int i=PROSITY-10; i<=PROSITY+20; i++)
                {
                    byteArraySendMessage[0] = SEND;
                    byteArraySendMessage[1] = SINGLE_MOVEMENT;
                    byteArraySendMessage[2] = CLOSE;
                    QTimer::singleShot(i, this, SLOT(buffer_send_message()));
                }
            }// посылка команды на закрытие
            stateChange = false;
            receiveShakesNumber += 1;
            timeGenerationCyclogram.start();
            qDebug() << "CyclogramStation = THRID_STATE";
        } else {
            if(receiveStrength >= MIN_CICLOGRAMM_STRENGHT || timeGenerationCyclogram.elapsed() >= MAX_CICLOGRAMM_STAGE_TIME){
                cyclogramStation = FOURTH_STATE;
                stateChange = true;
            }
        }
    }
    if(cyclogramStation == FOURTH_STATE){
        if(stateChange){
            timeGenerationCyclogram.start();
            stateChange = false;
            qDebug() << "CyclogramStation = FOURTH_STATE";
        } else {
            if(timeGenerationCyclogram.elapsed() >= receiveShakeTime || timeGenerationCyclogram.elapsed() >= MAX_CICLOGRAMM_STAGE_TIME){
                cyclogramStation = FIFTH_STATE;
                stateChange = true;
            }
        }
    }
    if(cyclogramStation == FIFTH_STATE){
        if(stateChange){
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
                    timeGenerationCyclogram.start();
                }
            }// посылка команды на остановку
            stateChange = false;
            qDebug() << "CyclogramStation = FIFTH_STATE";
        } else {
            if(timeGenerationCyclogram.elapsed() >= receiveCoolTime){
                cyclogramStation = FIRST_STATE;
                stateChange = true;
            }
        }
    }
}

void MainWindow::resetCyclogram() {
    cyclogramStation = FIRST_STATE;
    stateChange = true;
}

QTextStream stream (&fileLog);
QString buffer = "";
void MainWindow::writeToFileLog()
{
    if (fileLog.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        stream.readAll();
        stream <<"№" + QString::number(receiveShakesNumber)+"       Средний ток= " + QString::number(meanCurrent)+"       Максимальная сила= " + QString::number(printStrenght)+"       Температура= " + QString::number(printTemperature)+"       Уровень шума= " + QString::number(meanNoiseLevel)+"       Время сжатия= "+QString::number(timeOfIdleGrip)+"       Время= "+QDateTime::currentDateTime().toString("hh:mm")+"       Энергия на сват= "+QString::number(periodEnergy)+"       Оставшаяся энергия теоретически= "+QString::number(enrgyRestTheoretical)+"       Оставшаяся энергия расчитанна= "+QString::number(enrgyRestPractical)+"\n";
        fileLog.close();
    }
}


