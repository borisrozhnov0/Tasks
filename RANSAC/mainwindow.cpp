#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fr = ui->frame;

    installEventFilter(this);                                                       // Подключение EventFilter
    fr->installEventFilter(this);
    fr->setMouseTracking(true);

    connect(ui->StartButton, &QPushButton::clicked, this, &MainWindow::Start);      // Соединение слотов
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::actClose);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::actNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::SaveFile);
    connect(ui->actionSend, &QAction::triggered, this, &MainWindow::SendLine);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == fr)
    {
        if(event->type() == QEvent::MouseMove)                                      // Отслеживание относительного положения указателя мыши
        {
            static QString s;
            cursor_x = QWidget::mapFromGlobal(QCursor::pos()).x();
            cursor_y = QWidget::mapFromGlobal(QCursor::pos()).y();
            cursor_x = cursor_x < 0 ? 0 : cursor_x > 500 ? 500 : cursor_x;          // Ограничение курсора в области 500х500
            cursor_y = cursor_y < 0 ? 0 : cursor_y > 500 ? 500 : cursor_y;
            s = "X = " + QString::number(cursor_x);                                 // Вывод информации
            ui->labelX->setText(s);
            s = "Y = " + QString::number(cursor_y);
            ui->labelY->setText(s);
            return true;
        }
        if(event->type() ==  QEvent::MouseButtonPress)                              // Отслаживание нажатия клавиши мыши
        {
            QPoint new_p = QPoint(QWidget::mapFromGlobal(QCursor::pos()));
            new_p.setX(new_p.x() < 0 ? 0 : (new_p.x() > 500 ? 500 : new_p.x()));
            new_p.setY(new_p.y() < 0 ? 0 : (new_p.y() > 500 ? 500 : new_p.y()));
            if(ui->radioButton->isChecked())                                        // Добавление точки
                lp.append(new_p);
            if(ui->radioButton_2->isChecked())                                      // Удаление точки
            {
                for(auto it = lp.begin(); it != lp.end(); it++)
                    if(std::sqrt(std::pow(new_p.x()-it->x(),2) + std::pow(new_p.y()-it->y(),2)) <= 5.5)     // Поиск точки в радиусе 5.5
                    {
                        lp.erase(it);
                        break;
                    }
            }
            update();                                                               // Обновление
        }
    }
    return false;
}
// Асинхронный алгоритм
// warning: некорректно считает в случае, когда прямая горизонтальная/вертикальная
void MainWindow::StartAsync()
{
    std::list<std::thread> threads;
    std::vector<QLine> vl(lp.size()-1);                                     // Инициализация Хранилища линий
    std::vector<double> dist(lp.size()-1);                                  // Инициализация Хранилища общего расстояния от точек до линии
    int N = std::thread::hardware_concurrency();                            // Максимальное число поддерживаемых потоков
    N = N ? N : 1 ;
    QPoint q1, q2;
    auto func = [&](int i)                                                  // Лямбда-функция Асинхронного поиска расстояния
    {
        QPoint q1, q2;                                                      // Точки координат прямой

        for(auto p1 = lp.begin() + i, end = --lp.end(); p1 != end;)
        {
            double min = 1e16;                                                  // Минимальное расстояние
            for(auto p2 = p1+1; p2 != lp.end(); p2++)
            {
                double
                    A = 1/(double)(p2->x() - p1->x()),                          // Параметры прямой в виде Ax+By+C=0
                    B = 1/(double)(p1->y() - p2->y()),
                    C = (double)p1->y()/(double)(p2->y() - p1->y()) - (double)p1->x()/(double)(p2->x()-p1->x()),
                    SQRT = std::sqrt(A*A+B*B);
                double tmp=0;
                for(auto p = lp.begin(); p!= lp.end(); p++)                     // Вычисление расстояния от прямой до точек
                {
                    if(p1 == p || p2 == p) continue;
                    tmp += std::abs(A*p->x() + B* p->y() + C)/ SQRT;
                }
                if(tmp < min)
                {
                    min = tmp;
                    q1 = *p1;
                    q2 = *p2;
                }
            }
        vl[i] = {q1,q2};                                                         // Запись данных в общую память
        dist[i] = min;
        for(int j=0; j!=N;j++) if(p1 == --lp.end()) break; else p1++;
        i+=N;
        }
    };

    QTime time = QTime::currentTime();                                          // Запуск таймера
    for(int i = 0; i!= N; i++) threads.push_back(std::thread(func, i));         // Запуск потоков
    for(auto& t : threads) t.join();                                            // Ожидание завершения работы потоков
    threads.clear();

    int pos = -1;
    double min = 1e16;
    for(int i = 0; i!= vl.size(); i++)                                          // Поиск наименльшего результата
        if(dist[i] < min)
        {
            min = dist[i];
            pos = i;
        }
    unsigned int msec = time.msecsTo(QTime::currentTime());                     // Остановка таймера
    ui->label->setText(QString("Time: %1ms").arg(msec));
    setline = true;
    line = vl[pos];
    MainWindow::line_transform(line);                                          // Трансформация отрезка в линию
    update();
}

void MainWindow::Start()
{
    if(lp.size()<2)
    {
        QMessageBox * m = new QMessageBox;
        m->setText("Недостаточное число точек для запуска");
        m->show();
        return;
    }
    if(ui->checkAsync->isChecked())         // Проверка режима работы асинхронный/синхронный
    {
        StartAsync();
        return;
    }
    auto dist = [](const QPoint &p1, const QPoint &p2, const QPoint &p)
    {
        double  A = 1/(double)(p2.x() - p1.x()),
                B = 1/ (double)(p1.y() - p2.y()),
                C = (double)p1.y()/(double)(p2.y() - p1.y()) - (double)p1.x()/(double)(p2.x() - p1.x()),
                SQRT = std::sqrt(A*A + B*B);
        double ret = std::abs(A*p.x()+B*p.y()+C) / SQRT;
        return ret;
    };
    QPoint p1, p2;
    double min_dist = 1e16;
    // Синхронный алгоритм аналогичен асинхронному
    QTime time = QTime::currentTime();
    for(auto it1 = lp.begin(); it1 != --lp.end(); it1++)
    {
        for(auto it2 = it1+1; it2 != lp.end(); it2++)
        {
            double tmp = 0;
            for(auto it3 = lp.begin(); it3 != lp.end(); it3++)
            {
                if(it2==it3 || it1==it3) continue;
                tmp += dist(*it1, *it2, *it3);
            }
            if(tmp < min_dist)
            {
                min_dist = tmp >0 ? tmp: min_dist;
                p1 = *it1;
                p2 = *it2;
            }
        }
    }
    unsigned int msec = time.msecsTo(QTime::currentTime());
    ui->label->setText(QString("Time: %1ms").arg(msec));

    setline = true;
    line.setLine(p1.x(), p1.y(), p2.x(), p2.y());
    MainWindow::line_transform(line);
    update();
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    paint.begin(this);
    if(!setline)
    {
        paint.setBrush(QBrush(Qt::gray));  // Рисование Серых точек
        paint.setPen(QPen(Qt::gray));
        for(auto p: lp)
            paint.drawEllipse(p, 5, 5);
    }
    if(setline)
    {
        auto dist = [](const QLine &l, const QPoint p)
        {
            double ret = 0;
            double  A = 1/(double)(l.x2() - l.x1()),
                    B = 1/(double)(l.y1() - l.y2()),
                    C = (double)l.y1()/(double)(l.y2() - l.y1()) - (double)l.x1()/(double)(l.x2() - l.x1()),
                    SQRT = std::sqrt(A*A + B*B);
            ret = std::abs(A*p.x() + B * p.y() + C) / SQRT;
            return ret;
        };
        for(auto p: lp)                 // Рисование красныч/синих точек и синей прямой
        {
            if(dist(line, p) < 10)      // Если расстояние от прямой меньше 10px то рисуется синяя точка иначе красная
            {
                paint.setBrush(QBrush(Qt::blue));
                paint.setPen(QPen(Qt::blue));
            }
            else
            {
                paint.setBrush(QBrush(Qt::red));
                paint.setPen(QPen(Qt::red));
            }
            paint.drawEllipse(p, 5, 5);
        }
        paint.setBrush(QBrush(Qt::blue));
        paint.setPen(QPen(Qt::blue));
        paint.drawLine(line);
    }
    paint.end();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actNew()
{
    setline = false;
    lp.clear();
    update();
}

void MainWindow::actClose()
{
    this->close();
}
void MainWindow::setXY()
{
    setLabelXY(cursor_x, cursor_y);
}

// Изменение значения положения курсора
void MainWindow::setLabelXY(int x, int y)
{
    static QString S;
    S = "X: " + QString::number(x);
    ui->labelY->setText(S);
    S = "Y: " + QString::number(y);
    ui->labelX->setText(S);
}


// Трансформирование линии в отрезок
void MainWindow::line_transform(QLine &ln)
{
    int x1 = ln.x1(), x2 = ln.x2(), y1 = ln.y1(), y2=ln.y2();
    double  A = 1/(double)(x2 - x1),
            B = 1/(double)(y1 - y2),
            C = (double)y1/(double)(y2 - y1) - (double)x1/(double)(x2 - x1);
    QString line_func;
    line_func = A > 0 ? QString::number(std::abs(A)) : "-"+QString::number(std::abs(A));
    line_func += "x";
    line_func += B > 0 ? " + "+QString::number(std::abs(B)) : " - "+QString::number(std::abs(B));
    line_func += "y";
    line_func += C > 0 ? " + "+QString::number(std::abs(C)) : " - "+QString::number(std::abs(C));
    line_func += " = 0";
    ui->Linefunc->setText(line_func);
    double tmp;
    if(y1 > y2)
    {
        std::swap(x2,x1);
        std::swap(y1,y2);
    }
    // Продление отрезка по границ 33х500х20х500
    if(A == INFINITY)
    {
        x2 = x1;
        y1 = 33;
        y2 = 500;
    }
    else if(B == INFINITY)
    {
        y1 = y2;
        x1 = 500;
        x2 = 20;
    }
    else if(x1 > x2)
    {
        x1 = (-C / A);
        x1 = x1 > 500? 500 : x1;
        y1 = (-C-A*x1)/B;
        y1 = y1 < 32 ? 32 : y1;
        x1 = y1<33?(-C-B*y1)/A :x1;

        x2 = (-C - B*500) / A;
        x2 = x2 < 20 ? 20 : x2;
        y2 = (-C - A*x2) / B;
        y2 = y2 > 500 ? 500 : y2;
    }
    else
    {
        x1 = (-C / A);
        x1 = x1 < 20 ? 20 : x1;
        y1 = (-C-A*x1)/ B;
        y1 = y1 < 32 ? 32 : (-C - A*x1)/B;
        x1 = y1<33?(-C-B*y1)/A :x1;

        x2 = (-C - B*500) / A;
        x2 = x2 > 500 ? 500 : x2;
        y2 = (-C - A*x2) / B;
        y2 = y2 > 500 ? 500 : y2;

    }

    ln.setP1({x1, y1});
    ln.setP2({x2, y2});
}

// Стандартное открытие/сохранение файла
void MainWindow::OpenFile()
{
    fpath = QFileDialog::getOpenFileName(0, "Open sorce file", "", "*.txt");
    lp = parse();
    setline = false;
    update();
}
void MainWindow::SaveFile()
{
    fpath = QFileDialog::getSaveFileName(0, "Save project","","*.txt");
    QFile qf(fpath);
    if(!qf.open(QIODevice::WriteOnly)) return;
    QTextStream out(&qf);
    for(auto p : lp)
    {
        int x = p.x(), y = p.y();
        out << x << " " << y << "\n";
    }
    qf.close();
}

// Запуск отправки данных линии через UPD soket (!не тестировалась)
void MainWindow::SendLine()
{
    QWidget * w = new QWidget;

    QVBoxLayout * b = new QVBoxLayout; // Инициализация компонентов виджита
    QLabel * addr = new QLabel("Address");
    QLabel * port = new QLabel("Port");
    QLineEdit * laddr = new QLineEdit();
    QLineEdit * lport = new QLineEdit();
    QPushButton * ok = new QPushButton("Send");
    laddr->setText("127.0.0.1");
    lport->setText("4444");
    b->addWidget(addr);
    b->addWidget(laddr);
    b->addWidget(port);
    b->addWidget(lport);
    b->addWidget(ok);
    w->setLayout(b);

    connect(ok, &QPushButton::clicked, this, [=](){
        QUdpSocket sock;

        QByteArray b;
        b.append(QByteArray::number(line.p1().x())); // Создание датаграммы с координатами двух точек прин. прямой
        b.append(QByteArray::number(line.p1().y()));
        b.append(QByteArray::number(line.p2().x()));
        b.append(QByteArray::number(line.p2().y()));

        sock.writeDatagram(b, QHostAddress(laddr->text()), lport->text().toInt());
        sock.close();
        w->close();
    });
    w->show();
}
// Загрузка данных точек (!без проверки)
QList<QPoint> MainWindow::parse()
{
    QList<QPoint> ret;
    QFile qf(fpath);
    if(!qf.open(QIODevice::ReadOnly)) return ret;
    QTextStream input(&qf);
    while(!input.atEnd())
    {
        int x, y;
        input >> x;
        input >> y;
        ret.push_back({x,y});
    }
    qf.close();
    return ret;
}
