#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define RECEIVE_VARIABLES =

#include <QFile>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDateTime>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QtEndian>
#include <QDir>
#include <QTextStream>


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
    bool mCyclogramMode = false;
    bool mCyclogramStart = false;
    int printStrenght = 0;
    int printTemperature = 0;
    float meanCurrent = 0;
    float meanNoiseLevel = 0;
    float meanVoltage = 0;
    int lastMeanCycleVoltage = 0;
    int cyclogramStation = 1;
    float startVoltage = 0;
    float periodEnergy = 0;
    float sumPeriodEnergy = 0;
    float maxButteryEnergyCalculated = 0; //энергия, которую способна отдать не полностью заряженная батарея
    float enrgyRestPractical = 0; //энергия, которая осталась в батарейке
//    int enrgyRestTheoreticalCalculated = 0; //
    float enrgyRestTheoretical = 0; //энергия, которая осталась в батарейке, если считать батарейку полностью заряженной изначально
    int cycleMultiplier = 0;//необходим для продолжения счёта после превышения 65534 циклов(увеличивается на 1 с каждым циклом)
    QTimer timer_plot;

private slots:
    void serialRecieve();//получаем данные
    void set_stule();//наряжаем кнопочки и вьюхи
    void on_send_message_clicked();
    void on_edit_line_textChanged(const QString &arg1);
    void sendBytes (QString arg1);
    void separateSecondByte (QString secondByte);
    void on_connect_button_clicked();
    void update_ui();
    void add_graph();
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
    void on_contorl_mode_4_clicked();
    void on_servo_angle_valueChanged(int value);
    void on_send_nominal_temperature_textChanged(const QString &arg1);
    void on_send_stop_temperature_textChanged(const QString &arg1);
    void on_start_servo_angle_valueChanged(int value);
    void on_stop_servo_angle_valueChanged(int value);
    void on_test_HDLC_clicked();
    void realtimePlot();
    void generationCyclogram();
    void resetCyclogram();
};

#endif // MAINWINDOW_H
