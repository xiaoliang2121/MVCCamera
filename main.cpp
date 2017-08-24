#include "mvccamera.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MVCCamera w;
    w.show();

    return a.exec();
}
