#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDateTime>
#include <QList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void changeEvent(QEvent *e);
private slots:
    void on_searchInputEdit_textChanged(const QString &arg1);

    void on_outputTableView_doubleClicked(const QModelIndex &index);

    void on_addBtn_clicked();

    void on_deleteBtn_clicked();

    void on_importBtn_clicked();

    void on_exportBtn_clicked();

    void slotContextMenu(QPoint pos);

    void slot_Set15Reminder();

    void slot_Set30Reminder();

    void slot_SetOtherReminder();

    void slot_SetClearReminder();

    void slot_Modify();

    void closeEvent(QCloseEvent *e);

    void checkCurrentTime();

    void on_showMainAction();
    void on_exitAppAction();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void showToolTip(const QModelIndex &index);

    void on_action_FindRepeat_triggered();

    void on_action_FindAll_triggered();

    void on_action_MenuBar_Add_triggered();

    void on_action_MenuBar_Delete_triggered();

    void on_action_MenuBar_Import_triggered();

    void on_action_MenuBar_Export_triggered();

    void on_action_Menu_FindNoReminder_triggered();

    void on_action_Menu_FindToday_triggered();

    void on_action_Menu_Quit_triggered();

    void on_action_Menu_FindTimeup_triggered();

private:
    Ui::MainWindow *ui;
    QAction *set15Reminder;
    QAction *set30Reminder;
    QAction *setOtherReminder;
    QAction *setClearReminder;

    QAction *setModify;
    QMenu *popMenu;
    QSystemTrayIcon *mSysTrayIcon;

    QTimer *timer;

    QMenu *mMenu;
    QAction *mShowMainAction;
    QAction *mExitAppAction;

    void createActions();
    void createMenu();
    void deleteDateById(int id);
    QString getRemindDate(int id);

    //当前表格显示内容
    enum TABLECONTENT{
        CONTENT_ALL=0,      //查看全部人员
        CONTENT_REPEAT=1,   //查看重复人员
        CONTENT_NOREMIND=2, //查看没有提醒人员
        CONTENT_TODAY_REMIND=3,   //查看今日提醒人员
        CONTENT_TODAY_TIMEUP=4,   //查看今日到期人员
        CONTENT_OTHER=5     //其他人员
    };

    TABLECONTENT currentContent;
    QString whereCause;

    inline QString getTitle(TABLECONTENT tc){
        switch(tc){
        case CONTENT_ALL:
            return "全部人员";
        case CONTENT_REPEAT:
            return "重复人员";
        case CONTENT_NOREMIND:
            return "没有提醒人员";
        case CONTENT_TODAY_REMIND:
            return "今日提醒人员";
        case CONTENT_TODAY_TIMEUP:
            return "今日到期人员";
        case CONTENT_OTHER:
            return "筛选人员";
        default:
            return "其他";
        }
    }


private:
    bool checkSign = false;
    int minute = 0;

private:
    //数据库的关联表格 主库
    QSqlTableModel mShowDataModel;

    //提醒日期表格
    QSqlTableModel mReminderDateModel;
    void initSqlModel(const QString &order);
    void refreshSqlData();

    void initDateModel(int id);
    QList<int>* getReminderId(QString date);

    enum OPERATION_TYPE{
        OPERATION_DELETE = 0,
        OPERATION_UPDATE = 1
    };

    void updateRemindDate(OPERATION_TYPE operation,int id,const QString& date);
};

#endif // MAINWINDOW_H
