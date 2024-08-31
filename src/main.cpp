#include "mainwindow.h"

#include <QApplication>
#include <QDebug>


const char* version = "0.0.1";

int main(int argc, char *argv[])
{   
    qDebug() <<" version:" <<version ;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
