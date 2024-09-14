#include <QApplication>

#include "config.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    fun::config::init(&app);
    MainWindow w;
    w.show();
    return app.exec();
}
