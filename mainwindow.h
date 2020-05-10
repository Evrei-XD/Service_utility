#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define RECEIVE_VARIABLES =


#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray byteArreyReceiveMessage;
    QByteArray byteArraySendMessage;
    QString serialBuffer;
    QString productIdentifier;
    QString vendorIdentifier;
    bool sendFlag = true;

private slots:
    void serialRecieve();//получаем данные
    void set_stule();//наряжаем кнопочки и вьюхи
    void on_send_message_clicked();
    void on_edit_line_textChanged(const QString &arg1);
    void sendBytes (QString arg1);
    void separateSecondByte (QString secondByte);
    void on_connect_button_clicked();
    void update_ui();
    void on_start_clicked();
    void on_pause_clicked();
    void on_stop_clicked();
    void on_send_shake_time_textChanged(const QString &arg1);
    void buffer_send_message();
    void send_message();
};

#endif // MAINWINDOW_H
