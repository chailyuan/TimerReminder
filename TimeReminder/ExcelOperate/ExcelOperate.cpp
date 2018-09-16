#include "ExcelOperate.h"
#include <QDebug>


ExcelOperate::ExcelOperate()
{
}

ExcelOperate* ExcelOperate::mInstance=nullptr;

ExcelOperate *ExcelOperate::getInstance()
{
    if(mInstance==nullptr){
        mInstance=  new ExcelOperate;
    }
    return mInstance;
}

QAxObject* ExcelOperate::getExcel(const QString &workBooksName, const QString &sheetName)
{
    Q_UNUSED(sheetName)

    QString excel_file_path = QString("%1/../db/%2.xlsx").arg(QCoreApplication::applicationDirPath()).arg(workBooksName);
    excel_file_path = QDir::toNativeSeparators(excel_file_path);

    qDebug("111");


    QAxObject* m_excel = new QAxObject("Excel.Application");
    m_excel->setProperty("Visible", true);
    QAxObject* workbooks = m_excel->querySubObject("WorkBooks");
    workbooks->dynamicCall("Open (const QString&)", excel_file_path);
    QAxObject* workbook = m_excel->querySubObject("ActiveWorkBook");
    QAxObject* worksheets = workbook->querySubObject("WorkSheets");
    QAxObject* worksheet = workbook->querySubObject("Worksheets(int)", 1); //worksheet number
    QAxObject* usedrange = worksheet->querySubObject("UsedRange");
    QAxObject* rows = usedrange->querySubObject("Rows");
    QAxObject* columns = usedrange->querySubObject("Columns");
    int intRowStart = usedrange->property("Row").toInt();
    int intColStart = usedrange->property("Column").toInt();
    int intCols = columns->property("Count").toInt();
    int intRows = rows->property("Count").toInt();

    qDebug()<<intRowStart<<intRows;
    return worksheet;
}

QAxObject* ExcelOperate::getUserExcel(QString *type)
{
    Q_UNUSED(type)
    return getExcel("USER","sheet1");
}
