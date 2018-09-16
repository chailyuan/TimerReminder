#ifndef DBOPERATE_H
#define DBOPERATE_H

#include "dboperate_global.h"
#include <QtSql>

class DBOPERATESHARED_EXPORT DbOperate
{
public:
    static DbOperate *getInstance();
    QSqlDatabase getTimereminderDb(QString *type=nullptr);


private:
    DbOperate();
    static DbOperate* mInstance;
    QSqlDatabase getDb(const QString& connectionName,
                       const QString& serverIp, int port,
                       const QString& userId, const QString & password,
                       const QString& dbName, const QString & driver);
    bool checkDb(QSqlDatabase& db);
};

#endif // DBOPERATE_H
