#include "nmfMainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    nmfMainWindow w;
    w.show();

    return a.exec();
}
