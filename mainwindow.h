#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QByteArray byteArrey;//массив байт
    QString serialBuffer;

private slots:
    void serialRecieve();//получаем данные
    void serialRecieveFinish();//заканчиваем приём данных
    void set_stule();//наряжаем кнопочки и вьюхи
    void on_send_message_clicked();
    void on_edit_line_textChanged(const QString &arg1);
    void sendBytes (QString arg1);
    void separateSecondByte (QString secondByte);
    void on_connect_button_clicked();
};

#endif // MAINWINDOW_H
