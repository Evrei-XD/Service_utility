#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define RECEIVE_VARIABLES =

#include <QFile>
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
    int printStrenght = 0;
    int printTemperature = 0;
    int meanCurrent = 0;
    int meanNoiseLevel = 0;
    int meanVoltage = 0;
    int cycleMultiplier = 0;//необходим для продолжения счёта после превышения 65534 циклов(увеличивается на 1 с каждым циклом)

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
    void on_send_cool_time_textChanged(const QString &arg1);
    void on_send_stop_strength_textChanged(const QString &arg1);
    void on_send_stop_current_textChanged(const QString &arg1);
    void on_tenso_calib_clicked();
    void writeToFileLog();
    void on_open_clicked();
    void on_close_clicked();
    void on_invert_clicked();
    void on_mio_set_0_clicked();
    void on_contorl_mode_clicked();
    void on_contorl_mode_2_clicked();
    void on_contorl_mode_3_clicked();
    void on_servo_angle_valueChanged(int value);
    void on_send_nominal_temperature_textChanged(const QString &arg1);
    void on_send_stop_temperature_textChanged(const QString &arg1);
    void on_start_servo_angle_valueChanged(int value);
    void on_stop_servo_angle_valueChanged(int value);
    void on_test_HDLC_clicked();
};

#endif // MAINWINDOW_H
