#include "AddDialog.h"
#include "ui_adddialog.h"
#include <QDebug>
#include <QMessageBox>

AddDialog::AddDialog(const RecordEntity& re,QWidget *parent) :
    QDialog(parent),
    ep(re),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);

    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->lineEdit_ID );

    ui->lineEdit_ID->setValidator(validator);

    qDebug()<<ep.toString();

    if(ep.getId() != 0){
        ui->lineEdit_ID->setText(QString("%1").arg(ep.getId()));
        ui->lineEdit_PARTMENT->setText(ep.getPartment());
        ui->lineEdit_NAME->setText(ep.getName());
        ui->comboBox_SEX->setCurrentText(ep.getSex());
        ui->comboBox_DEPTH->setCurrentText(ep.getDepth());
        ui->dateEdit_LASTTIME->setDate(QDate::fromString(ep.getLasttime(),"yyyy/MM/dd"));
        ui->dateEdit_DEADLINE->setDate(QDate::fromString(ep.getDeadline(),"yyyy/MM/dd"));
        ui->textEdit_REMARK->setText(ep.getRemark());
    }
    else{
        ui->dateEdit_DEADLINE->setDate(QDate::currentDate());
        ui->dateEdit_LASTTIME->setDate(QDate::currentDate());
    }
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::on_confirmBtn_clicked()
{

    int id = ui->lineEdit_ID->text().toInt();
    QString partment = ui->lineEdit_PARTMENT->text();
    QString name = ui->lineEdit_NAME->text();
    QString sex = ui->comboBox_SEX->currentText();
    QString depth = ui->comboBox_DEPTH->currentText();
    QString lasttime = ui->dateEdit_LASTTIME->date().toString("yyyy/MM/dd");
    QString deadline = ui->dateEdit_DEADLINE->date().toString("yyyy/MM/dd");
    QString remark = ui->textEdit_REMARK->toPlainText();

    if(id == 0){
        //提示
        QMessageBox message(QMessageBox::NoIcon, "Error", "输入框不能为空！", QMessageBox::Yes, nullptr);
        message.exec();
        return;
    }

    re->setId(id);
    re->setPartment(partment);
    re->setName(name);
    re->setSex(sex);
    re->setDepth(depth);
    re->setLasttime(lasttime);
    re->setDeadline(deadline);
    re->setRemark(remark);

    this->done(0);
}

void AddDialog::on_cancelBtn_clicked()
{
    this->done(-1);
}
