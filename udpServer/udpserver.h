#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QWidget>
#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QDebug>
#include <QPushButton>
#include <QBitArray>
#include <QTextEdit>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QLineEdit>
#include <regex>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
    class server;
    class client;
}
QT_END_NAMESPACE

class server : public QMainWindow
{
    Q_OBJECT
public:
    server(unsigned int port, QWidget *parent = nullptr);
    void sndD();
    ~server();
public slots:
    void setAddres();
    void setPort();
protected:
    bool value_check();
    bool correct_range(int, int, int, int, int, int, int);
private:
    QLineEdit *lad, * lport, *lx, *ly, *lv, *lm, *ls, *la, *lp, *lr;
    QString Addres_s;
    QPushButton * snd;
    QUdpSocket * sock;
    Ui::server *ui;
    QHostAddress host_adr;
    int port_num;
};


class client : public QMainWindow
{
    Q_OBJECT

public:
    client(int port, QWidget *parent = nullptr);
    ~client();
private slots:
    void read();
    void setSock();
private:
    QString Standart_answere =
        "Координаты: X = %1, Y = %2\n"
        "Скорость: V = %3, Ускорение A = %4, Мощность P = %5\n"
        "Режим M = %6, Состояние S = %7\n";
    QUdpSocket * sock;
    QTextEdit * te;
    Ui::client *ui;
};
#endif // UDPSERVER_H
