#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include "RecordEntity.h"
#include <QCloseEvent>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(const RecordEntity& re,QWidget *parent = nullptr);
    ~AddDialog();

private:
    RecordEntity * re;
    RecordEntity ep;//入参

public:
    inline void setRe(RecordEntity * re){this->re = re;}
    inline RecordEntity* getRe()const {return re;}

    inline void setEp(const RecordEntity& _ep){this->ep = _ep;}

private slots:
    void on_confirmBtn_clicked();

    void on_cancelBtn_clicked();

    void closeEvent(QCloseEvent *e);

    void reject();

private:
    Ui::AddDialog *ui;
};

#endif // ADDDIALOG_H
