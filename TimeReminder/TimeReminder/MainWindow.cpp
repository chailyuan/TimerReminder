#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "DbOperate.h"
#include "DatePick.h"
#include "AddDialog.h"
#include "RecordEntity.h"
#include <QMessageBox>
#include "ExcelOperate.h"
#include <QFileDialog>


#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer(this)),
    minute(QTime::currentTime().minute()),
    mShowDataModel(nullptr, DbOperate::getInstance()->getTimereminderDb(nullptr)),
    mReminderDateModel(nullptr, DbOperate::getInstance()->getTimereminderDb(nullptr))
{
    ui->setupUi(this);

    setModify = new QAction (this);
    setModify->setText(QObject::tr("修改"));

    set15Reminder = new QAction ( this);
    set15Reminder->setText(QObject::tr("设置15天的提醒"));

    set30Reminder = new QAction ( this);
    set30Reminder->setText(QObject::tr("设置30天的提醒"));

    setOtherReminder = new QAction ( this);
    setOtherReminder->setText(QObject::tr("设置其他日期提醒"));

    popMenu = new QMenu(ui->outputTableView);
    popMenu->addAction(setModify);
    popMenu->addAction(set15Reminder);
    popMenu->addAction(set30Reminder);
    popMenu->addAction(setOtherReminder);

    connect(
                ui->outputTableView,
                SIGNAL(customContextMenuRequested(QPoint)),
                this,
                SLOT(slotContextMenu(QPoint))
                );

    connect(
                setModify,
                SIGNAL(triggered()),
                this,
                SLOT(slot_Modify())
                );

    connect(
                set15Reminder,
                SIGNAL(triggered()),
                this,
                SLOT(slot_Set15Reminder())
                );

    connect(
                set30Reminder,
                SIGNAL(triggered()),
                this,
                SLOT(slot_Set30Reminder())
                );

    connect(
                setOtherReminder,
                SIGNAL(triggered()),
                this,
                SLOT(slot_SetOtherReminder())
                );


    connect(
                timer,
                SIGNAL(timeout()),
                this,
                SLOT(checkCurrentTime())
                );
    timer->start(1000);


    initSqlModel("");
    initDateModel(0);

    //系统托盘图标
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon icon = QIcon("../icon/start.png");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QObject::tr("test"));

    mSysTrayIcon->show();

    //给QSystemTrayIcon添加槽函数
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    //建立托盘操作的菜单
    createActions();
    createMenu();
    //在系统托盘显示此对象
    mSysTrayIcon->show();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e){
    Q_UNUSED(e)
    e->ignore();
    this->hide();
}

void MainWindow::changeEvent(QEvent *e)
{
    Q_UNUSED(e)
}

void MainWindow::createActions()
{
    mShowMainAction = new QAction(QObject::tr("显示主界面"),this);
    connect(mShowMainAction,SIGNAL(triggered()),this,SLOT(on_showMainAction()));

    mExitAppAction = new QAction(QObject::tr("退出"),this);
    connect(mExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));

}

void MainWindow::createMenu()
{
    mMenu = new QMenu(this);
    //新增菜单项---显示主界面
    mMenu->addAction(mShowMainAction);
    //增加分隔符
    mMenu->addSeparator();
    //新增菜单项---退出程序
    mMenu->addAction(mExitAppAction);
    //把QMenu赋给QSystemTrayIcon对象
    mSysTrayIcon->setContextMenu(mMenu);
}
/*
* 当在系统托盘点击菜单内的显示主界面操作
*/
void MainWindow::on_showMainAction()
{
    this->show();
}

/*
* 当在系统托盘点击菜单内的退出程序操作
*/
void MainWindow::on_exitAppAction()
{
    exit(0);
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //显示消息球，1s后自动消失
        //第一个参数是标题
        //第二个参数是消息内容
        //第三个参数图标
        //第四个参数是超时毫秒数
        mSysTrayIcon->showMessage(QObject::tr("Message Title"),
                                  QObject::tr("欢迎使用此程序"),
                                  QSystemTrayIcon::Information,
                                  1000);
        break;
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        break;
    }
}

void MainWindow::checkCurrentTime()
{
    QDateTime time = QDateTime::currentDateTime();
    int minute = time.time().minute();
    qDebug()<<"minute:"<<minute;
    if(minute == this->minute){
        if(checkSign == false){
            checkSign = true;
            //TODO
            initDateModel(0);
            qDebug()<<"1111111111";
            qDebug()<<QDate::currentDate().toString("yyyy/MM/dd");

            QList<int> *list = getReminderId(QDate::currentDate().toString("yyyy/MM/dd"));
            qDebug()<<list->size();
            if(list->size() != 0){
                //select name from USER where ID = ?;

                QMessageBox message(QMessageBox::Question, "Error", "有用户到提醒日期,是否显示", QMessageBox::Yes|QMessageBox::No, nullptr);
                if(message.exec() == QMessageBox::No){
                    return;
                }

                QString str = QString("ID='%1'").arg(list->at(0));
                for(int i=1;i<list->size();++i){
                    str += QString("or ID='%1'").arg(list->at(i));
                }
                qDebug()<<str;
                initSqlModel(str);
            }
        }
    }else{
        checkSign = false;
    }
}

void MainWindow::slotContextMenu(QPoint pos){
    QModelIndex index = ui->outputTableView->indexAt(pos);

    if (index.isValid())
    {
        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标位置
    }
}

void MainWindow::slot_Set15Reminder()
{
    qDebug()<<"slot_SetReminder";
    int curRow = ui->outputTableView->currentIndex().row();
    int id = mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt();
    QString dateStr = mShowDataModel.data(mShowDataModel.index(curRow,6)).toString();
    QDate date = QDate::fromString(dateStr,"yyyy/MM/dd");
    QDate remindDate = date.addDays(-15);
    QString remindDateStr = remindDate.toString("yyyy/MM/dd");

    updateRemindDate(id,remindDateStr);
}


void MainWindow::slot_Set30Reminder()
{
    qDebug()<<"slot_SetReminder";
    int curRow = ui->outputTableView->currentIndex().row();
    int id = mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt();
    QString dateStr = mShowDataModel.data(mShowDataModel.index(curRow,6)).toString();
    QDate date = QDate::fromString(dateStr,"yyyy/MM/dd");
    QDate remindDate = date.addDays(-30);
    QString remindDateStr = remindDate.toString("yyyy/MM/dd");

    updateRemindDate(id,remindDateStr);
}

void MainWindow::slot_SetOtherReminder()
{
    qDebug()<<"slot_SetReminder";
    int curRow = ui->outputTableView->currentIndex().row();

    QString date;
    DatePick dp(mShowDataModel.data(mShowDataModel.index(curRow,6)).toString());
    dp.setDate(&date);
    int returnStatus = dp.exec();
qDebug()<<date;

    int id = mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt();
    qDebug()<<"id = "<<id;
    if(returnStatus != 0)
        return;
    updateRemindDate(id,date);
}

void MainWindow::slot_Modify()
{
    qDebug()<<"slot_Modify";
    int curRow = ui->outputTableView->currentIndex().row();
    RecordEntity modifyEntity(
                mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt(),
                mShowDataModel.data(mShowDataModel.index(curRow,1)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,2)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,3)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,4)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,5)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,6)).toString(),
                mShowDataModel.data(mShowDataModel.index(curRow,7)).toString());

    RecordEntity backEntity ;
    AddDialog ad(modifyEntity);
    ad.setRe(&backEntity);
    int returnStatus = ad.exec();

    if(returnStatus != 0)
        return;

    mShowDataModel.setData(mShowDataModel.index(curRow,0),backEntity.getId());
    for(int i=1;i<8;++i){
        mShowDataModel.setData(mShowDataModel.index(curRow,i),backEntity.getPara(i));
    }
    mShowDataModel.submitAll(); //可以直接提交
}

void MainWindow::updateRemindDate(int id,const QString& date){
    initDateModel(id);
    int rowNum = mReminderDateModel.rowCount();

    qDebug()<<rowNum;

    if(rowNum > 0){
        mReminderDateModel.removeRows(0,rowNum);
    }

    initDateModel(0);
    rowNum = mReminderDateModel.rowCount();
    qDebug()<<rowNum;
    mReminderDateModel.insertRow(rowNum);
    mReminderDateModel.setData(mReminderDateModel.index(rowNum,0),id);
    mReminderDateModel.setData(mReminderDateModel.index(rowNum,1),date);

    mReminderDateModel.submitAll(); //可以直接提交
}

void MainWindow::initDateModel(int id){
    mReminderDateModel.setTable("REMIND_DATE");
    mReminderDateModel.setEditStrategy(QSqlTableModel::OnFieldChange);

    ui->outputTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if(id != 0)
        mReminderDateModel.setFilter(QString("ID=%1").arg(id));
    mReminderDateModel.setSort(0,Qt::AscendingOrder);
    mReminderDateModel.select();
}

QList<int>* MainWindow::getReminderId(QString date)
{
    QList<int> *list = new QList<int>();
    mReminderDateModel.setTable("REMIND_DATE");
    mReminderDateModel.setEditStrategy(QSqlTableModel::OnFieldChange);

    ui->outputTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    mReminderDateModel.setFilter(QString("DATE='%1'").arg(date));

    mReminderDateModel.setSort(0,Qt::AscendingOrder);
    mReminderDateModel.select();

    for(int i=0;i<mReminderDateModel.rowCount();++i){
        list->append(mReminderDateModel.data(mReminderDateModel.index(i,0)).toInt());
    }
    initDateModel(0);

    return list;
}

void MainWindow::initSqlModel(const QString &order){

    mShowDataModel.setTable("USER");
    mShowDataModel.setEditStrategy(QSqlTableModel::OnFieldChange);

    ui->outputTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if(order!="")
        mShowDataModel.setFilter(order);
    mShowDataModel.setSort(0,Qt::AscendingOrder);
    mShowDataModel.select();
    refreshSqlData();
}
void MainWindow::refreshSqlData(){
    ui->outputTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->outputTableView->horizontalHeader()->setMinimumSectionSize(100);
    ui->outputTableView->setAlternatingRowColors(true);
    ui->outputTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->outputTableView->setModel(&mShowDataModel);

    ui->outputTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    mShowDataModel.setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
    mShowDataModel.setHeaderData(1,Qt::Horizontal,QObject::tr("部门"));
    mShowDataModel.setHeaderData(2,Qt::Horizontal,QObject::tr("姓名"));
    mShowDataModel.setHeaderData(3,Qt::Horizontal,QObject::tr("性别"));
    mShowDataModel.setHeaderData(4,Qt::Horizontal,QObject::tr("涉密程度"));
    mShowDataModel.setHeaderData(5,Qt::Horizontal,QObject::tr("上次审查日期"));


    mShowDataModel.setHeaderData(6,Qt::Horizontal,QObject::tr("到期日期"));
    mShowDataModel.setHeaderData(7,Qt::Horizontal,QObject::tr("备注"));
}

void MainWindow::on_searchInputEdit_textChanged(const QString &arg1)
{
    if(arg1=="")
        return initSqlModel("");

    QString order = QString("ID='%1' or PARTMENT like '%")
                        .arg(arg1)
                    +QString("%2%' or NAME like '%")
                        .arg(arg1)
                    +QString("%3%'")
                        .arg(arg1);


    qDebug()<<order;
    initSqlModel(order);
}

void MainWindow::on_outputTableView_doubleClicked(const QModelIndex &index)
{
    int columnIndex = index.column();
    if(columnIndex == 5 || columnIndex == 6){
        QString date;
        DatePick dp(mShowDataModel.data(index).toString());
        dp.setDate(&date);
        dp.exec();
        if(date  != ""){
            //更新数据
            mShowDataModel.setData(index,date);
            mShowDataModel.submitAll(); //可以直接提交
        }
    }

}

void MainWindow::on_addBtn_clicked()
{
    RecordEntity re,ep;
    AddDialog ad(ep);
    ad.setRe(&re);
    ad.exec();
    qDebug()<<re.toString();

    if(re.getId() == 0)
        return;

    int rowNum = mShowDataModel.rowCount(); //获得表的行数

    mShowDataModel.insertRow(rowNum);
    mShowDataModel.setData(mShowDataModel.index(rowNum,0),re.getId());
    for(int i=1;i<8;++i){
        mShowDataModel.setData(mShowDataModel.index(rowNum,i),re.getPara(i));
    }
    mShowDataModel.submitAll(); //可以直接提交
}

void MainWindow::on_deleteBtn_clicked()
{
    int curRow = ui->outputTableView->currentIndex().row();

    int id = mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt();

    mShowDataModel.removeRow(curRow); //删除一行

    initDateModel(id);
    int rowNum = mReminderDateModel.rowCount();
    if(rowNum > 0){
        mReminderDateModel.removeRows(0,rowNum);
    }
    initDateModel(0);


    initSqlModel("");
}

void MainWindow::on_importBtn_clicked()
{

    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开图片"));
    //设置默认文件路径

    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd("excel");
    QString path=dir.path();//获取程序当前目录

    fileDialog->setDirectory(path);
    //设置文件过滤器
    fileDialog->setNameFilter(tr("Images(*.xlsx *.xls)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径

    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }

    //
    QString sql = "select * from ut_s_orders";

    initSqlModel("");
    int rowNum = mShowDataModel.rowCount();
    int max = 0;
    if(rowNum == 0){
        max = 1;
    }else{
        max = mShowDataModel.data(mShowDataModel.index(rowNum-1,0)).toInt();
    }

    qDebug()<<"max = "<<max;



    for(auto tmp:fileNames){
        Document xlsx(tmp); // load excel file
        if (!xlsx.isLoadPackage()) {
            return; // failed to load excel
        }

        int quit = 0;
        for(int row=2;quit<1;++row){
            for(int col=1;col<=8;++col){
                qDebug()<<"col = "<<col;
                Cell* cell = xlsx.cellAt(row, col);
                if ( cell == nullptr || cell->readValue().toString()==""){
                    quit = 1; // cell vaule is not set
                    break;
                }
                 QVariant var = cell->readValue();
                 qDebug()<<var;

                 if(col == 1){
                     mShowDataModel.insertRow(rowNum+row-2);
                     mShowDataModel.setData(mShowDataModel.index(rowNum+row-2,0),QVariant(max+row-1));
                 }else if(col ==6 || col == 7){
                     mShowDataModel.setData(mShowDataModel.index(rowNum+row-2,col-1),cell->readValue().toDate().toString("yyyy/MM/dd"));
                 }else{
                     mShowDataModel.setData(mShowDataModel.index(rowNum+row-2,col-1),cell->readValue().toString());

                 }
            }
            mShowDataModel.submitAll(); //可以直接提交
        }
    }

}

void MainWindow::on_exportBtn_clicked()
{
    initSqlModel("");
    int rowCount = mShowDataModel.rowCount();

    QXlsx::Document xlsx;

    xlsx.write("A1", "序号");
    xlsx.write("B1", "部门");
    xlsx.write("C1", "姓名");
    xlsx.write("D1", "性别");
    xlsx.write("E1", "涉密程度");
    xlsx.write("F1", "上次审查日期");
    xlsx.write("G1", "到期日期");
    xlsx.write("H1", "备注");

    for(int i=0;i<rowCount;++i){
        for(int j=0;j<mShowDataModel.columnCount();++j){
            xlsx.write(CellReference(i+2,j+1),QVariant(mShowDataModel.data(mShowDataModel.index(i,j)).toString()));
        }

    }
    xlsx.saveAs("../excel/USER.xlsx"); // save the document as 'USER.xlsx'


    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd("excel");
    QString path=dir.path();//获取程序当前目录
    path.replace("/","\\");//将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    QProcess::startDetached("explorer "+path);//打开上面获取的目录

}
