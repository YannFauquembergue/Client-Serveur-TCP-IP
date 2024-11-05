#include "ClientTCPIP.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientTCPIP w;
    w.show();
    return a.exec();
}
