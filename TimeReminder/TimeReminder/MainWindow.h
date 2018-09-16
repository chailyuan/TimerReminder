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

    void slot_Modify();

    void closeEvent(QCloseEvent *e);

    void checkCurrentTime();

    void on_showMainAction();
    void on_exitAppAction();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    QAction *set15Reminder;
    QAction *set30Reminder;
    QAction *setOtherReminder;
    QAction *setModify;
    QMenu *popMenu;
    QSystemTrayIcon *mSysTrayIcon;

    QTimer *timer;

    QMenu *mMenu;
    QAction *mShowMainAction;
    QAction *mExitAppAction;

    void createActions();
    void createMenu();

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
    void updateRemindDate(int id,const QString& date);
};

#endif // MAINWINDOW_H
