#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ExcelOperate.h"
#include "DbOperate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QSqlDatabase db = DbOperate::getInstance()->getTimereminderDb(nullptr);
    qDebug()<<db;
}
