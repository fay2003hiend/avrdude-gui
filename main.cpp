#include "widget.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::setCurrent(a.applicationDirPath());

    Widget w;
    w.show();

    return a.exec();
}
