#include "ConfigInterface.h"
#include <QCoreApplication>

ConfigInterface* ConfigInterface::mInstance = nullptr;

ConfigInterface *ConfigInterface::getInstance()
{
    if(mInstance==nullptr){
        mInstance = new ConfigInterface;
    }
    return mInstance;
}

const QVariant ConfigInterface::readDatabase(QString sec, QString key, QVariant defaultVal) const
{
    QVariant ret = databaseSettings.value(QString("%1/%2").arg(sec).arg(key),defaultVal);
    return ret;
}

void ConfigInterface::writeDatabase(QString sec, QString key, QVariant val)
{
    databaseSettings.setValue(QString("%1/%2").arg(sec).arg(key),val);
}

ConfigInterface::ConfigInterface():
    databaseSettings(QString("%1/../ini/database.ini").arg(QCoreApplication::applicationDirPath()),
                     QSettings::IniFormat)
{
    databaseSettings.setIniCodec("UTF-8");
}

