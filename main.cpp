#include "SupplyServer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SupplyServer w;
    w.show();

    return a.exec();
}
