#ifndef EXCELOPERATE_H
#define EXCELOPERATE_H

#include "exceloperate_global.h"
#include <QAxObject>
#include <QtCore>

class EXCELOPERATESHARED_EXPORT ExcelOperate
{



public:
    static ExcelOperate *getInstance();
    QAxObject* getUserExcel(QString *type=nullptr);

private:
    ExcelOperate();
    static ExcelOperate* mInstance;
    QAxObject* getExcel(const QString& workBooksName,
                       const QString& sheetName);
    bool checkExcel(ExcelOperate& excel);
};

#endif // EXCELOPERATE_H
