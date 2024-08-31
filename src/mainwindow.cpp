#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blah.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb0_clicked(){
    do_blah();
    qDebug() << "buttong Clicked";
}
