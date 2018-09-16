#ifndef DATEPICK_H
#define DATEPICK_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class DatePick;
}

class DatePick : public QDialog
{
    Q_OBJECT

public:
    explicit DatePick(const QString& _date = nullptr,QWidget *parent = nullptr);
    ~DatePick();

private slots:
    void on_calendarWidget_selectionChanged();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    QString *newDate;
    QString oriDate;

public:
    void setDate(QString* d){this->newDate = d;}
    QString *getDate(){return this->newDate;}

private:
    Ui::DatePick *ui;
};

#endif // DATEPICK_H
