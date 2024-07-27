#include "Line.h"

Line::Line(const QPoint & p1, const QPoint & p2):Line(p1.x(), p1.y(), p2.x(), p2.y()){ }
Line::Line(const QLine & l):Line(l.x1(), l.y1(), l.x2(), l.y2()){ }
Line::Line(const Line & l):A(l.A),B(l.B), C(l.C){ }
Line::Line(int x1, int y1, int x2, int y2)
{
    if(x1 == x2)
    {
        A = 1.0;
        B = 0.0;
        C = -x1;
    }
    else if(y1 == y2)
    {
        A = 0.0;
        B = 1.0;
        C = -y1;
    }
    else
    {
        A = 1.0 / (x2 - x1);
        B = 1.0 / (y1 - y2);
        C = (double)x1 / (x1 - x2) + (double)y1 / (y2 - y1);
    }
}

std::string Line::to_stdstr()
{
    std::string str;
    if(A)
    {
        str += std::to_string(A);
        str += "x";
        if(B)
        {
            str += B < 0 ? " - " : " + ";
            str += std::to_string(std::abs(B));
            str += "y";
        }
    }
    else
    {
        str += std::to_string(B);
    }
    if(C)
    {
        str += C < 0 ? " - " : " + ";
        str += std::to_string(std::abs(C));
    }
    str += " = 0";
    return str;
}

QLine Line::tp_qline(int x_min, int y_min, int x_max, int y_max)
{
    if(!A)
    {
        y_min = -C;
        y_max = -C;
    }
    else if(!B)
    {
        x_min = -C;
        x_max = -C;
    }
    else
    {
        int xm1 = x_min, ym1 = -( A * xm1 + C) / B;
        int ym2 = y_min, xm2 = -( B * ym2 + C) / A;
        if(ym1<y_min)
            x_min = xm2;
        else
            y_min = ym1;

        xm1 = x_max, ym1 = -( A * xm1 + C) / B;
        ym2 = y_max, xm2 = -( B * ym2 + C) / A;
        if(ym1>y_max)
            x_max = xm2;
        else
            y_max = ym1;
    }
    return QLine(x_min, y_min, x_max, y_max);
}

double Line::distance(const QPoint & p)
{
    double SQRT = 1.0 / std::sqrt(A*A + B*B);
    return std::abs(A * p.x() + B * p.y() + C) * SQRT;
}

double Line::distance(const QVector<QPoint> & v)
{
    double _dst = 0.0, SQRT = 1.0 / std::sqrt(A*A + B*B);
    for(auto it = v.begin(); it!= v.end(); it++) _dst += std::abs(A * it->x() + B * it->y() + C) * SQRT;
    return _dst;
}

Line Line::RANSAC(const QList<QPoint> & l, int inl, double percent)
{
    Line ln;
    QVector<QPoint> v(l);                                                       // Переводим спсиок точек в массив

    int n = v.size();
    int size = __comp(n,2);
                                                                               // Определяем максимальное число итераций
    int k =(int)(std::log(1.0 - percent / 100.0) / std::log(1.0 - (double)__comp(inl,2)/size));
    k = k>size ? size : k;
    int l_ar[3][size];                                                         // Массив комбинаций пар точек
    for(int i =0, pos = 0; i!= n-1; i++)
        for(int j = i+1; j!= n; j++)
        {
            l_ar[0][pos] = i;
            l_ar[1][pos] = j;
            l_ar[2][pos++] = 1;
        }
    std::random_device rd;                                                     // Задаем генератор случайных чисел
    std::mt19937 gen(rd());                                                    // от 0 до n-1 вкл.
    int i = 0;

    double min_dst = 1e16;
    while(k--)
    {
        std::uniform_int_distribution<> dist(i, size-1);
        int pos = dist(gen), p1 = l_ar[0][pos], p2 = l_ar[1][pos];
        std::swap(l_ar[0][i], l_ar[0][pos]);
        std::swap(l_ar[1][i], l_ar[1][pos]);
        i = i > (size-1) ? size-1 : i+1;
        Line tmp_ln(v[p1], v[p2]);
        double tmp_dst = tmp_ln.distance(v);
        if(tmp_dst < min_dst)
        {
            ln = tmp_ln;
            min_dst = tmp_dst;
        }
    }
    return ln;
}

Line Line::asyncRANSAC(const QList<QPoint> & l, int inl, double percent)
{
    Line ln;
    QVector<QPoint> v(l);
    int n = v.size();
    int size = __comp(n,2);
                                                                                // Определяем максимальное число итераций
    int k =(int)(std::log(1.0 - percent / 100.0) / std::log(1.0 - (double)__comp(inl,2)/size));
    std::atomic<int> ak(k>size ? size : k);
    std::vector<int> v1, v2, v3;        // Массив комбинаций пар точек
    for(int i =0; i!= n-1; i++)
        for(int j = i+1; j!= n; j++)
        {
            v1.push_back(i);
            v2.push_back(j);
            v3.push_back(1);
        }
    int th_n = std::thread::hardware_concurrency();
    Line th_line[th_n];
    double th_dist[th_n];
    std::random_device rd;
    std::mutex mt;
    std::atomic<int> i = 0;
    auto thread_func = [&](int id)
    {
        th_dist[id] = 1e16;
        std::mt19937 gen(rd());                                                    // от 0 до n-1 вкл.
        while(ak-->0)
        {
            mt.lock();
            std::uniform_int_distribution<> dist(i, size-1);
            int pos = dist(gen), p1 = v1[pos], p2 = v2[pos];
            std::swap(v1[i], v1[pos]);
            std::swap(v2[i], v2[pos]);
            if(i++ == size) {
                mt.unlock();
                break;
            }
            mt.unlock();
            Line tmp_ln(v[p1], v[p2]);
            double tmp_dst = tmp_ln.distance(v);
            if(tmp_dst < th_dist[id] )
            {
                th_dist[id] = tmp_dst;
                th_line[id] = tmp_ln;
            }
        }
    };
    std::list<std::thread> th_list;
    for(int id = 0; id != th_n; id++)th_list.push_back(std::thread(thread_func, id));
    for(auto & th : th_list) th.join();
    double min_dist = th_dist[0];
    ln = th_line[0];
    for(int i = 0; i != th_n; i++)
        if(min_dist > th_dist[i])
        {
            min_dist = th_dist[i];
            ln = th_line[i];
        }
    return ln;
}

int __fuc(int n)
{
    if(n == 0) return 1;
    if(n < 3) return n;
    int mul = 1;
    while(n != 1) mul *= n--;
    return mul;
}
int __comp(int n, int m)
{
    if (n <= m)return 1;
    int a = n, b = m, c = n-m;
    int _comp = 1;
    if(b > c)
    {
        while(a != b) _comp *= a--;
        _comp /= __fuc(c);
    }
    else
    {
        while(a != c) _comp *= a--;
        _comp /= __fuc(b);
    }
    return _comp;
}
