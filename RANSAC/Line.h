#ifndef LINE_H
#define LINE_H

#include <QPoint>
#include <QLine>
#include <QDebug>
#include <QVector>
#include <cmath>
#include <random>
#include <QTime>
#include <array>
#include <ctime>
#include <thread>
#include <mutex>

class Line
{
public:
    Line(const QPoint & p1, const QPoint & p2);
    Line(const QLine & l);
    Line(int x1, int y1, int x2, int y2);
    Line(const Line & l);
    ~Line()=default;
    std::string to_stdstr();
    QLine tp_qline(int x_min, int y_min, int x_max, int y_max);
    static Line RANSAC(const QList<QPoint> & l, int n, double percent);
    static Line asyncRANSAC(const QList<QPoint> & l, int inl, double percent);
    Line operator=(const QLine l){ return Line(l); }
    double distance(const QVector<QPoint> & p);
    double distance(const QPoint & p);
protected:
    Line() = default;
    double A, B, C;
private:
};

int __fuc(int n);
int __comp(int n, int m);

#endif // LINE_H
