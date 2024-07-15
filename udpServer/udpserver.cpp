#include "udpserver.h"
#include "./ui_server.h"
#include "./ui_client.h"

server::server(unsigned int num, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::server)
    , port_num(num)
{
    sock = new QUdpSocket(this);                                    // Инициализация udpСокета
    host_adr = QHostAddress("127.0.0.1");

    ui->setupUi(this);                                              // Инициализация ссылок на Ui
    snd = ui->pushButton;
    lad = ui->lineEditAd;
    lport = ui->lineEditPort;
    lx = ui->lineEditX;
    ly = ui->lineEditY;
    lv = ui->lineEditV;
    lm = ui->lineEditM;
    ls = ui->lineEditS;
    la = ui->lineEditA;
    lp = ui->lineEditP;
    lr = ui->lineEditR;

    connect(ui->pushButton, &QPushButton::clicked, this, &sndD);    // Соединение слотов и сигналов
    connect(lad, &QLineEdit::textEdited, this, &setAddres);
    connect(lport, &QLineEdit::textEdited, this, &setPort);

    ui->lineEditAd->setText("127.0.0.1");
    ui->lineEditPort->setText(QString::number(num));
}
/* Проверка значений полей lineEdit на корректное заполнение.
 * В случае ошибки инициализирует QMessageBox с расшифровкой
 * Значения в полях Address и Port не проверяются
 */
bool server::value_check()
{
    bool correct = true;
    std::regex  uint_ex("^[0-9]{1,3}$"),                            // Инициализация регулярных выражений для проверки значений
                int_ex("^-?[0-9]{1,3}$"),
                bit_ex("^(0|1){16}$"),
                th_ex("^-?[0-9]{1,3}\\.[0-9]$");
    QString Error_MSG("Ошибка:\n\n");

    if(!std::regex_match((lx->text()).toStdString(), uint_ex))
    {
        correct = false;
        Error_MSG += "\t- Координата X - целое число от 0 до 63\n";
    }
    if(!std::regex_match((ly->text()).toStdString(), int_ex))
    {
        correct = false;
        Error_MSG += "\t- Координата Y - целое число от -32 до 31\n";
    }
    if(!std::regex_match((lv->text()).toStdString(), uint_ex))
    {
        correct = false;
        Error_MSG += "\t- Скорость - целое число от 0 до 255\n";
    }
    if(!std::regex_match((lm->text()).toStdString(), uint_ex))
    {
        correct = false;
        Error_MSG += "\t- Режим работы - целое число от 0 до 3\n";
    }
    if(!std::regex_match((ls->text()).toStdString(), uint_ex))
    {
        correct = false;
        Error_MSG += "\t- Состояние - целое число от 0 до 3\n";
    }
    if(!std::regex_match((la->text()).toStdString(), th_ex))
    {
        correct = false;
        Error_MSG += "\t- Ускорение - дробное число от -12.7 до 12.8\n";
    }
    if(!std::regex_match((lp->text()).toStdString(), uint_ex))
    {
        correct = false;
        Error_MSG += "\t- Мощнось - целое число от 0 до 130\n";
    }
    if(!std::regex_match((lr->text()).toStdString(), bit_ex))
    {
        correct = false;
        Error_MSG += "\t- Резерв должен состоять из 16 бит\n";
    }
    if(!correct)                                                           // Вызов сообщения об ошибке, с расшифровкой
    {
        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText(Error_MSG);
        msg.exec();
    }
    return correct;
}

/* Проверка диапазона значений полей lineEdit на корректное заполнение.
 * В случае ошибки инициализирует QMessageBox с расшифровкой
 */
bool server::correct_range(int x, int y, int v, int m, int s, int a, int p)
{
    bool correct = true;
    auto inr = [](int src, int frst, int scnd){ return (src < frst || src > scnd) ? false : true; };    // Проверка диапазона
    QString Error_MSG("Ошибка:\n\n");
    if(!inr(x, 0, 63))
    {
        correct = false;
        Error_MSG+= "\t - Координата X должна быть в пределах от 0 до 63";
    }
    if(!inr(y, -32, 31))
    {
        correct = false;
        Error_MSG+= "\t - Координата Y должна быть в пределах от -32 до 31";
    }
    if(!inr(v, 0, 255))
    {
        correct = false;
        Error_MSG+= "\t - Скорость должна быть в пределах от 0 до 255";
    }
    if(!inr(m, 0, 3))
    {
        correct = false;
        Error_MSG+= "\t - Режим работы должен быть в пределах от 0 до 3";
    }
    if(!inr(s, 0, 3))
    {
        correct = false;
        Error_MSG+= "\t - Состояние должно быть в пределах от 0 до 3";
    }
    if(!inr(a, -127, 128))
    {
        correct = false;
        Error_MSG+= "\t - Ускорение должно быть в пределах от -12.7 до 12.8";
    }
    if(!inr(p, 0, 130))
    {
        correct = false;
        Error_MSG+= "\t - Ускорение должно быть в пределах от 0 до 130";
    }
    if(!correct)                                                                //  Вывод сообщения об ошибке
    {
        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText(Error_MSG);
        msg.exec();
    }
    return correct;
}

/* Проверка значений полей и отправка зашифрованой датаграммы
 */
void server::sndD()
{
    auto bstoi= [](const QString & s) { int ret = 0; for(int i = 0; i!=16; i++) ret |= (s[i]=='0'? 0 : 1) << i; return ret; };  // QString->short

    if(!value_check()) return;  // Проверка корректного значения
    QString st = la->text();
    st.erase(st.end() - 2);     // преобразование поля ускорения хх.х -> xxx

    int x = lx->text().toInt(),
        y = ly->text().toInt(),
        v = lv->text().toInt(),
        m = lm->text().toInt(),
        s = ls->text().toInt(),
        p = lp->text().toInt(),
        a = st.toInt();
    if(!correct_range(x,y,v,m,s,a,p)) return;   // Проверка диапазона значений

    /* Создание датаграммы
     *
     * бит  | 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
     * ---------------------------------------------------------------------|
     * w1   | 0 | 0 | х | х | х | х | х | х | 0 | 0 | х | х | х | х | х | х |
     * ---------------------------------------------------------------------|
     * w2   | 0 | 0 | х | х | х | х | х | х | x | x | х | х | х | х | х | х |
     * ---------------------------------------------------------------------|
     * w3   | x | x | х | х | х | х | х | х | x | x | х | х | х | х | х | х |
     * ---------------------------------------------------------------------|
     * w4   | x | x | х | х | х | х | х | х | x | x | х | х | х | х | х | х |
     * ---------------------------------------------------------------------|
     *
     * w1 - содерщит координаты х, y
     * w2 - содерщит скорость, режим, состояние
     * w3 - ускорение и мощность
     * w4 - резерв
     */
    short w1 = 0, w2 = 0, w3 = 0, w4 = 0;
    w1 = (short)x << 8;
    w1 |=(short)y & 0b111111;
    w2 = (short)v << 4;
    w2 |= ((short)m & 0b11) << 2;
    w2 |= ((short)s) & 0b11;
    w3 = (short)a << 8;
    w3 |= (short)p & 0xFF;
    w4 = bstoi(lr->text());

    QByteArray dtg;             // Создание битового массива
    dtg.append(w1>>8);
    dtg.append(w1);
    dtg.append(w2>>8);
    dtg.append(w2);
    dtg.append(w3>>8);
    dtg.append(w3);
    dtg.append(w4>>8);
    dtg.append(w4);

    sock->writeDatagram(dtg, host_adr, port_num);   // Отправка датаграммы
}

server::~server()
{
    sock->close();
    delete ui;
}
/* Изменение Адреса
 * Проверка корректности ввода не производится
 */
void server::setAddres()
{
    Addres_s = lad->text();
    host_adr = QHostAddress(Addres_s);
}
/* Изменение Порта
 * Проверка корректности ввода не производится
 */
void server::setPort()
{
    QString new_port = lport->text();
    port_num = new_port.toInt();
}

client::client(int port, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::client)
{

    ui->setupUi(this);
    setWindowTitle(QString("Client, Port:%1").arg(port));
    sock = new QUdpSocket(this);
    sock->bind(QHostAddress::LocalHost, port);
    connect(sock, &sock->readyRead, this, &read);
    connect(ui->pushButton, &QPushButton::clicked, this, &setSock);
    te = ui->textEdit;
    ui->lineEdit->setText(QString::number(port));
}

void client::setSock()
{
    int num = ui->lineEdit->text().toInt();
    sock->close();
    sock->bind(QHostAddress::LocalHost, num);
    setWindowTitle(QString("Client, Port:%1").arg(num));
}

void client::read()
{
    QByteArray dtg;
    do
    {
        dtg.resize(sock->pendingDatagramSize());
        sock->readDatagram(dtg.data(), dtg.size());
    }while(sock->hasPendingDatagrams());

    short x, y, v, m, s, a, p, r;
    x = dtg[0];
    y = dtg[1] > 31 ? dtg[1]-64: dtg[1];
    v = dtg[2] << 4;
    v |= (dtg[3] >> 4)&0b00001111;
    m = (dtg[3] & 0b00001100) >> 2;
    s = dtg[3] & 0b00000011;
    a = dtg[4];
    p = dtg[5] & 0xFF;
    r = ((dtg[6]<<8) & 0xFF00) | (dtg[7] & 0xFF);

    QString sa;
    if(a < 0 & a != 128) sa += "-";
    sa += QString::number(abs(a/10));
    sa+=".";
    sa+=QString::number(abs(a%10));

    QString answere = Standart_answere.arg(x).arg(y).arg(v).arg(sa).arg(p).arg(m).arg(s);
    answere += "Резерв: ";
    for(int i = 15; i!= -1; --i)
        answere += r&(1<<(15-i))?"1":"0";
    answere += "\n";

    te->moveCursor(QTextCursor::End);
    te->insertPlainText(answere);
    te->moveCursor(QTextCursor::End);
}

client::~client()
{
    sock->close();
    delete ui;
}
