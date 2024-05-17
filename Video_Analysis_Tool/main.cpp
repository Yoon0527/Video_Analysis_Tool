#include "Video_Analysis_Tool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Video_Analysis_Tool w;
    w.show();
    return a.exec();
}
