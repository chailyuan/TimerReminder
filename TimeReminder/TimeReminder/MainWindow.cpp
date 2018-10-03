#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "DbOperate.h"
#include "DatePick.h"
#include "AddDialog.h"
#include "RecordEntity.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QToolTip>
#include <QDesktopServices>


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

    setClearReminder = new QAction ( this);
    setClearReminder->setText(QObject::tr("清除提醒"));

    popMenu = new QMenu(ui->outputTableView);
    popMenu->addAction(setModify);
    popMenu->addAction(set15Reminder);
    popMenu->addAction(set30Reminder);
    popMenu->addAction(setOtherReminder);
    popMenu->addAction(setClearReminder);

    connect(
                ui->outputTableView,
                SIGNAL(customContextMenuRequested(QPoint)),
                this,
                SLOT(slotContextMenu(QPoint))
                );

    //鼠标移动事件
    ui->outputTableView->setMouseTracking(true);
    connect(
                ui->outputTableView,
                SIGNAL(entered(const QModelIndex&)),
                this,
                SLOT(showToolTip(const QModelIndex&))
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
                setClearReminder,
                SIGNAL(triggered()),
                this,
                SLOT(slot_SetClearReminder())
                );




    connect(
                timer,
                SIGNAL(timeout()),
                this,
                SLOT(checkCurrentTime())
                );
    timer->start(1000);

    whereCause = "";
    currentContent = CONTENT_ALL;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
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

void MainWindow::showToolTip(const QModelIndex &index)
{
        if(!index.isValid())
                return;

        int row = index.row();
        int id = mShowDataModel.data(mShowDataModel.index(row,0)).toInt();

        QToolTip::showText(QCursor::pos(), getRemindDate(id));
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
        mSysTrayIcon->showMessage(QObject::tr("提醒器"),
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
    if(minute == this->minute){
        if(checkSign == false){
            checkSign = true;
            //TODO
            initDateModel(0);


            QList<int> *list = getReminderId(QDate::currentDate().toString("yyyy/MM/dd"));

            if(list->size() != 0){
                //select name from USER where ID = ?;

                QMessageBox message(QMessageBox::Question, "Error", "有用户到提醒日期,是否显示", QMessageBox::Yes|QMessageBox::No, nullptr);
                if(message.exec() == QMessageBox::No){
                    return;
                }

                //显示今日到期人员
                on_action_Menu_FindToday_triggered();
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

    updateRemindDate(OPERATION_UPDATE,id,remindDateStr);
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

    updateRemindDate(OPERATION_UPDATE, id,remindDateStr);
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
    updateRemindDate(OPERATION_UPDATE,id,date);
}

void MainWindow::slot_SetClearReminder()
{
    int curRow = ui->outputTableView->currentIndex().row();

    int id = mShowDataModel.data(mShowDataModel.index(curRow,0)).toInt();

    updateRemindDate(OPERATION_DELETE,id,"");
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

/**
 * @brief MainWindow::getRemindDate
 * @param id 人员对应的id
 */
QString MainWindow::getRemindDate(int id)
{
    initDateModel(id);
    int rowNum = mReminderDateModel.rowCount();

//    qDebug()<<rowNum;

    if(rowNum > 0){
        return QString("提醒日期:")+mReminderDateModel.data(mReminderDateModel.index(0,1)).toString();
    }
    return "无提醒";
}

/**
 * @brief MainWindow::updateRemindDate
 * @param operation 操作类型，0删除，1更改
 * @param id 序号
 * @param date 更改的日期
 */
void MainWindow::updateRemindDate(OPERATION_TYPE operation,int id,const QString& date){
    initDateModel(id);
    int rowNum = mReminderDateModel.rowCount();

    qDebug()<<rowNum;

    if(rowNum > 0){
        mReminderDateModel.removeRows(0,rowNum);
    }

    if(operation == OPERATION_DELETE) {
        return;
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

    //设置id居中排列
}

void MainWindow::setAlignment(int column)
{

}

void MainWindow::on_searchInputEdit_textChanged(const QString &arg1)
{
    if(arg1=="") {
        whereCause = "";
        currentContent = CONTENT_ALL;
        return initSqlModel(whereCause);
        setLabelContent(getTitle(currentContent));
    }

   whereCause = QString("ID='%1' or PARTMENT like '%")
                        .arg(arg1)
                    +QString("%2%' or NAME like '%")
                        .arg(arg1)
                    +QString("%3%' or SEX like '%")
                        .arg(arg1)
                    +QString("%4%' or DEPTH like '%")
                        .arg(arg1)
                    +QString("%5%'")
                        .arg(arg1);

    currentContent = CONTENT_OTHER;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
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
    //添加人员的时候必须是显示全部
    if(currentContent != CONTENT_ALL){
        QMessageBox message(
                    QMessageBox::Question,
                    "提醒",
                    "添加人员的时候必须先显示全部人员，是否显示？",
                    QMessageBox::Yes|QMessageBox::No, nullptr);
        if(message.exec() == QMessageBox::No){
            return;
        }

        whereCause = "";
        currentContent = CONTENT_ALL;
        initSqlModel(whereCause);
        setLabelContent(getTitle(currentContent));
    }
    //
    int rowNum = mShowDataModel.rowCount(); //获得表的行数
    int curMaxId = mShowDataModel.data(mShowDataModel.index(rowNum-1,0)).toInt();

    //ep入参，re回参
    RecordEntity re,ep;

    ep.setId(curMaxId+1);

    AddDialog ad(ep);
    ad.setRe(&re);
    ad.exec();
    qDebug()<<re.toString();

    if(re.getId() == 0)
        return;



    mShowDataModel.insertRow(rowNum);
    mShowDataModel.setData(mShowDataModel.index(rowNum,0),re.getId());
    for(int i=1;i<8;++i){
        mShowDataModel.setData(mShowDataModel.index(rowNum,i),re.getPara(i));
    }
    mShowDataModel.submitAll(); //可以直接提交
}

void MainWindow::on_deleteBtn_clicked()
{
    //删除数据库中的选中行
    QItemSelectionModel *selections = ui->outputTableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    QMap<int, int> rows;
    foreach (QModelIndex index, selected)
       rows.insert(index.row(), mShowDataModel.data(mShowDataModel.index(index.row(),0)).toInt());
    QMapIterator<int, int> r(rows);
    r.toBack();
    while (r.hasPrevious()) {
           r.previous();
       mShowDataModel.removeRow(r.key());
       deleteDateById(r.value());
    }
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
//    //得到id 并删除数据库数据

//    int curRow = ui->tableView->currentIndex().row();

//    QModelIndex index = ui->tableView->currentIndex();

//    int id=index.sibling(curRow,7).data().toInt();

//    //删除数据库中数据

//    QSqlQuery query;

//        query.prepare("delete from table where id =:id ");

//        query.bindValue("id",id);

//        query.exec();

//        if(!query.isActive()){

//            QMessageBox::critical(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("删除数据失败!"));

//            return;

//        }

}

void MainWindow::deleteDateById(int id)
{
    //删除提醒表里的相关数据
    initDateModel(id);
    int rowNum = mReminderDateModel.rowCount();
    if(rowNum > 0){
        mReminderDateModel.removeRows(0,rowNum);
    }
    initDateModel(0);


    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
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

    whereCause = "";
    currentContent = CONTENT_ALL;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));

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

    QMessageBox message(
                QMessageBox::Question,
                "提醒",
                QString("导出")+getTitle(currentContent)+QString("?"),
                QMessageBox::Yes|QMessageBox::No, nullptr);
    if(message.exec() == QMessageBox::No){
        return;
    }

    int rowCount = mShowDataModel.rowCount();

    if(rowCount == 0)
    {
        QMessageBox message1(
                    QMessageBox::Question,
                    "提醒",
                    QString("要导出的数据列表为空，还要导出吗？"),
                    QMessageBox::Yes|QMessageBox::No, nullptr);
        if(message1.exec() == QMessageBox::No){
            return;
        }
    }

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

    QString applicationPath = QCoreApplication::applicationDirPath().mid(0,QCoreApplication::applicationDirPath().indexOf("Run"));
    applicationPath += "Run/excel/";

    QString fileName = applicationPath+getTitle(currentContent)+(".xlsx");
    qDebug()<<fileName;
    xlsx.saveAs(fileName); // save the document as 'USER.xlsx'

    QDesktopServices::openUrl(QUrl::fromLocalFile(applicationPath));

}


void MainWindow::on_action_MenuBar_Add_triggered()
{
    on_addBtn_clicked();
}

void MainWindow::on_action_MenuBar_Delete_triggered()
{
    on_deleteBtn_clicked();
}

void MainWindow::on_action_MenuBar_Import_triggered()
{
    on_importBtn_clicked();
}

void MainWindow::on_action_MenuBar_Export_triggered()
{
    on_exportBtn_clicked();
}


void MainWindow::on_action_FindRepeat_triggered()
{
    whereCause = "(USER.NAME) in  "
                 "(select NAME from USER "
                 "group by NAME  having count(*) > 1)";
    currentContent = CONTENT_REPEAT;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
}

void MainWindow::on_action_FindAll_triggered()
{
    whereCause = "";
    currentContent = CONTENT_ALL;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
}

void MainWindow::on_action_Menu_FindNoReminder_triggered()
{
    whereCause = "(USER.ID) not in  "
                 "(select distinct ID "
                 "from REMIND_DATE)";
    currentContent = CONTENT_NOREMIND;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
}

void MainWindow::on_action_Menu_FindToday_triggered()
{
    QString today = QDate::currentDate().toString("yyyy/MM/dd");
    whereCause = QString("(USER.ID) in  "
                            "(select distinct ID from REMIND_DATE "
                            "where REMIND_DATE.DATE='%1')").arg(today);

    currentContent = CONTENT_TODAY_REMIND;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
}

void MainWindow::on_action_Menu_FindTimeup_triggered()
{
    QString today = QDate::currentDate().toString("yyyy/MM/dd");
    whereCause = QString("(USER.DEADLINE='%1')").arg(today);

    currentContent = CONTENT_TODAY_TIMEUP;
    initSqlModel(whereCause);
    setLabelContent(getTitle(currentContent));
}

void MainWindow::on_action_Menu_Quit_triggered()
{
    exit(0);
}

void MainWindow::setLabelContent(QString content)
{
    ui->label_CurrentContent->setText(content);
}

