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

private slots:
    void serialRecieve();//получаем данные
    void set_stule();//наряжаем кнопочки и вьюхи
    void on_send_message_clicked();
    void on_edit_line_textChanged(const QString &arg1);
    void sendByte (QString arg1);
};

#endif // MAINWINDOW_H
