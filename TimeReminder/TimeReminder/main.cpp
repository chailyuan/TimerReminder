#include "MainWindow.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory mem("TimeReminder-V1.0");//以系统exe名称为参数，定义共享内存mem
    if(!mem.create(1))//创建共享内存mem，如果该共享内存已存在，则弹出提示对话框，并退出
    {
        QMessageBox::information(
                    0,
                    QObject::tr("Warning"),
                    QObject::tr("程序已经在运行，请勿重复打开"));

        //打开已经运行的程序

        return 0;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
