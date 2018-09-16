#include "DatePick.h"
#include "ui_datepick.h"
#include "QDebug"
#include <QString>

DatePick::DatePick(const QString& _date,QWidget *parent) :
    QDialog(parent),
    oriDate(_date),
    ui(new Ui::DatePick)
{
    ui->setupUi(this);
    QDate d = QDate::fromString(oriDate,"yyyy/MM/dd");
    ui->calendarWidget->setCurrentPage(d.year(),d.month());
}

DatePick::~DatePick()
{
    delete ui;
}

void DatePick::on_calendarWidget_selectionChanged()
{
    *newDate = ui->calendarWidget->selectedDate().toString("yyyy/MM/dd");
}

void DatePick::on_buttonBox_accepted()
{
    *newDate = ui->calendarWidget->selectedDate().toString("yyyy/MM/dd");
    this->done(0);
}

void DatePick::on_buttonBox_rejected()
{
    this->done(-1);
}
