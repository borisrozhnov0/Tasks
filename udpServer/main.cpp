#include "udpserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    server w(4444);
    w.show();
    client c(4444);
    c.show();
    return a.exec();
}
