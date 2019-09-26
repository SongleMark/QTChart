#include "configservice.h"
#include "logger/logger.h"
#include <QTextCodec>

#define FILEPATH  "/home/masongle/my_qt/MyProject/OICQ/config.ini"
#define SDATABASE "Storage/Database"
#define STABLE    "Storage/Table"
#define SHOSTNAME "Storage/HostName"
#define SUSERNAME "Storage/UserName"
#define SPASSWORD "Storage/Password"
#define SPORT     "Storage/Port"
#define SIP       "Server/IP"
#define SERPORT   "Server/Port"
#define SISSERVER "Server/IsServer"
#define IMAGEPATH "Path/Image"
#define TAG "ConfigService"

ConfigService *ConfigService::mInstance = new ConfigService();

ConfigService *ConfigService::GetInstance()
{
    return mInstance;
}

ConfigService::ConfigService()
{

}

ConfigService::~ConfigService()
{

}

//read mysql information from config.ini
void ConfigService::ReadFromConfigFile(Storage *storage)
{
    //create readfile pointer
    QSettings *readfile = new QSettings(FILEPATH, QSettings::IniFormat);
    readfile->setIniCodec("UTF-8");//set can read chonese
    storage->database = readfile->value(SDATABASE).toString();
    storage->hostname = readfile->value(SHOSTNAME).toString();
    storage->password = readfile->value(SPASSWORD).toString();
    storage->port = readfile->value(SPORT).toInt();
    storage->table = readfile->value(STABLE).toString();
    storage->username = readfile->value(SUSERNAME).toString();

    delete readfile;
}

//read tcp communication information from config.ini
void ConfigService::ReadFromConfigFile(Communication *communication)
{
    //create readfile pointer
    QSettings *readfile = new QSettings(FILEPATH, QSettings::IniFormat);
    readfile->setIniCodec("UTF-8");//set can read chonese
    communication->ip = readfile->value(SIP).toString();
    communication->port = readfile->value(SERPORT).toString();
    communication->isserver = readfile->value(SISSERVER).toString();

    delete readfile;
}

//read image path from config.ini
QString ConfigService::ReadFromConfigFile()
{
    //create readfile pointer
    QSettings *readfile = new QSettings(FILEPATH, QSettings::IniFormat);
    //readfile->setIniCodec(QTextCodec::codecForName("GB2312"));
    readfile->setIniCodec("UTF-8");//set can read chonese
    QString path = readfile->value(IMAGEPATH).toString();
    LOG(TAG, "read image path is ", path);

    return path;
}
