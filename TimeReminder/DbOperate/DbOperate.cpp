#include "DbOperate.h"
#include "ConfigInterface.h"

const QString INI_SEC_DATABASE = "password";
const QString INI_DB_USERNAME = "username";
const QString INI_DB_PASSWORD = "password";
const QString INI_DB_SERVERIP = "serverip";
const QString INI_DB_SERVERPORT = "serverport";
const QString INI_DB_DBNAME = "dbname";
const QString INI_DB_DRIVER = "driver";
const QString INI_DB_CONNECTIONNAME = "connectionname";

DbOperate::DbOperate()
{
}



DbOperate* DbOperate::mInstance=nullptr;

DbOperate *DbOperate::getInstance()
{
    if(mInstance==nullptr){
        mInstance=  new DbOperate;
    }
    return mInstance;
}

QSqlDatabase DbOperate::getTimereminderDb(QString *type)
{
    Q_UNUSED(type);
    return getDb(INI_DB_CONNECTIONNAME,
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_SERVERIP,"127.0.0.1").toString(),
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_SERVERPORT,"3306").toInt(),
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_USERNAME,"root").toString(),
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_PASSWORD,"123456").toString(),
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_DBNAME,"timereminder").toString(),
                 ConfigInterface::getInstance()->readDatabase(INI_SEC_DATABASE,INI_DB_DRIVER,"QSQLITE").toString()
                 );
}

QSqlDatabase DbOperate::getDb(const QString &connectionName,
                              const QString &serverIp, int port,
                              const QString &userId, const QString &password,
                              const QString &dbName, const QString &driver)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if(!db.isValid()){
        db = QSqlDatabase::addDatabase(driver,connectionName);
    }
    if(db.isOpen() && checkDb(db)){
        return db;
    }
    QString sDbName = dbName;
    if(driver.compare("QSQLITE",Qt::CaseInsensitive)==0){
        sDbName =QString("%1/../db/%2.db").arg(QCoreApplication::applicationDirPath()).arg(dbName);
    }
    db.setHostName(serverIp);
    db.setPort(port);
    db.setDatabaseName(sDbName);
    db.setUserName(userId);
    db.setPassword(password);

    bool ret = db.open();
    if(ret){
        db.exec("SET NAMES 'utf8'");
    }
    qDebug()<<"数据库打开成功";
    return db;
}

bool DbOperate::checkDb(QSqlDatabase &db)
{
    QSqlQuery query("select 1",db);
    return !(query.lastError().isValid());
}
