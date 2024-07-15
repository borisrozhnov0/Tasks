#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <Qobject>
#include <QWidget>
#include <QAbstractButton>
#include <QMouseEvent>
#include <QFrame>
#include <QPainter>
#include <QTime>
#include <thread>
#include <array>
#include <QFile>
#include <QRegularExpression>
#include <QLineEdit>
#include <QtNetwork/QUdpSocket>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class point: public QAbstractButton
{
public:
    point();
    ~point();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool eventFilter(QObject *obj, QEvent *event);
protected slots:
    void actClose();
    void actNew();
    void setXY();
    void Start();
    void StartAsync();
    void OpenFile();
    void SaveFile();
    void SendLine();
protected:
    QList<QPoint> parse();              // Лист Координат точек
    int cursor_x, cursor_y;             // Положение курсора
    void line_transform(QLine &line);
private:
    QString fpath;
    void paintEvent(QPaintEvent* event);
    void setLabelXY(int x, int y);
    Ui::MainWindow *ui;
    QFrame * fr;
    QPainter paint;
    QList<QPoint> lp;
    QLine line;                         // Линия
    bool setline = false;
};
#endif // MAINWINDOW_H
