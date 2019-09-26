#include "storageservice.h"
#include "logger/logger.h"
#include <QMessageBox>

#define TAG "StorageService"

StorageService *StorageService::mInstance = nullptr;

StorageService *StorageService::GetInstance()
{
    if(nullptr == mInstance)
    {
        mInstance = new StorageService();
    }

    return mInstance;
}

StorageService::StorageService()
{

}

StorageService::~StorageService()
{
    DisConnected();
    if(nullptr != mQuery)
        delete mQuery;
}

//连接数据库
bool StorageService::Connected()
{
    mDb = QSqlDatabase::addDatabase("QMYSQL");
    mDb.setDatabaseName(mMysql.database);
    mDb.setHostName(mMysql.hostname);
    mDb.setPassword(mMysql.password);
    mDb.setPort(mMysql.port);
    mDb.setUserName(mMysql.username);

    if(!mDb.open())
    {
        QMessageBox::warning(nullptr, QObject::tr("失败"), QObject::tr("数据库打开失败"));
        return false;
    }
    mQuery = new QSqlQuery(mDb);

    return true;
}

//断开与数据库的连接
void StorageService::DisConnected()
{
    mQuery->clear();
    mDb.close();
}

#if 0
QString name;     //姓名
QString user;     //用户名
QString password; //密码
QString sex;      //性别
QString address;  //户籍地
QString emil;     //邮箱
QString imagepath;//头像地址
QString signature;//个性签名
int64_t tel;      //电话
int64_t Id;       //身份证号
QString birthday; //出生日期
#endif
//向数据库写入注册信息
bool StorageService::WriteToStorage(const UserInfo &info)
{
    QStringList tables = mDb.tables();
    if(!tables.contains(mMysql.table))
    {
        QString sql = QString("CREATE TABLE %1 (name char(32), user char(32), password char(32),"
                              "sex char(12), address char(40), emil char(32), imagepath char(32),"
                              "signature char(66), tel bigint, id bigint, birthday char(32));").arg(mMysql.table);
        LOG(TAG, "sql = ", sql);
        if(!mQuery->exec(sql))
        {
            LOG(TAG, "exec error in sql = ", sql);
            return false;
        }
    }
    QString check = QString("select count(*) from %1 where user='%2';").arg(mMysql.table).arg(info.user);
    LOG(TAG, "sql = ", check);
    if(!mQuery->exec(check))
    {
        LOG(TAG, "exec error in check...");
        return false;
    }
    if(mQuery->next())
    {
        int count = mQuery->value(0).toInt();
        if(count >= 1)
        {
            //QMessageBox::warning(nullptr, QObject::tr("失败"), QObject::tr("已存在用户名为%1的用户").arg(info.user));
            LOG(TAG, "已存在", info.user);
            return false;
        }
    }

    QString sql = QString("INSERT INTO %1 values('%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', %10, %11, '%12');")
                   .arg(mMysql.table).arg(QString(QLatin1String(info.name)))
                   .arg(QString::fromLocal8Bit(info.user)).arg(QString::fromLocal8Bit(info.password))
                   .arg(QString::fromLocal8Bit(info.sex)).arg(QString::fromLocal8Bit(info.address))
                   .arg(QString::fromLocal8Bit(info.emil)).arg(QString::fromLocal8Bit(info.imagepath))
                   .arg(QString::fromLocal8Bit(info.signature)).arg(info.tel).arg(info.Id)
                   .arg(QString::fromLocal8Bit(info.birthday));
    LOG(TAG, "sql = ", sql);
    if(!mQuery->exec(sql))
    {
        LOG(TAG, "exec error in sql = ", sql);
        return false;
    }

    return true;
}

//从数据库比对用户输入的用户名与密码
bool StorageService::ReadFromStorage(const char *user, const char *password)
{
    QString PassWord;
    char *apassword;
    QString sql = QString("select password from %1 where user='%2';").arg(mMysql.table).arg(user);
    LOG(TAG, "sql = ", sql);
    if(!mQuery->exec(sql))
    {
        LOG(TAG, "exec error in sql = ", sql);
        return false;
    }
    if(mQuery->next())
    {
        PassWord = mQuery->value(0).toString();
        QByteArray byte = PassWord.toLatin1();
        apassword = byte.data();
        LOG(TAG, "query password  = ", apassword);
    }
    if(0 != strcmp(apassword ,password))
    {
        return false;
    }

    return true;
}

//根据用户名获取用户本身信息
UserInfo StorageService::ReadFromStorage(const QString &username)
{
    UserInfo user;
    QString sql = QString("select * from %1 where user='%2';").arg(mMysql.table).arg(username);
    LOG(TAG, "sql = ", sql);
    if(mQuery->exec(sql))
    {
        if(mQuery->next())
        {
            //name
            std::string stdname = mQuery->value(0).toString().toStdString();
            const char *name = stdname.c_str();
            strcpy(user.name, name);
            //username
            QByteArray byteuser = username.toLatin1();
            char *userna = byteuser.data();
            strcpy(user.user, userna);
            //password
            QByteArray bytepassword = mQuery->value(2).toString().toLatin1();
            char *password = bytepassword.data();
            strcpy(user.password, password);
            //sex
            std::string stdsex = mQuery->value(3).toString().toStdString();
            const char *sex = stdsex.c_str();
            strcpy(user.sex, sex);
            LOG(TAG, "read user sex = ", user.sex);
            //address
            std::string strdaddress = mQuery->value(4).toString().toStdString();
            const char *address = strdaddress.c_str();
            strcpy(user.address, address);
            LOG(TAG, "户籍: ", user.address);
            //emil
            QByteArray byteemil = mQuery->value(5).toString().toLatin1();
            char *emil = byteemil.data();
            strcpy(user.emil, emil);
            //image
            std::string stdimage = mQuery->value(6).toString().toStdString();
            const char *image = stdimage.c_str();
            strcpy(user.imagepath, image);
            LOG(TAG, "image path = ", user.imagepath);
            //signature
            std::string stdsignature = mQuery->value(7).toString().toStdString();
            const char *signature = stdsignature.data();
            strcpy(user.signature, signature);
            //tel
            user.tel = mQuery->value(8).toString().toLong();
            //id
            user.Id = mQuery->value(9).toString().toLong();
            //birthday
            QByteArray bytebirthday = mQuery->value(10).toString().toLatin1();
            char *birthday = bytebirthday.data();
            strcpy(user.birthday, birthday);
        }
    }
    else
    {
        LOG(TAG, "exec sql error ...");
    }

    return user;
}
//从manager类获取configservice指针
void StorageService::GetConfigFromManager(ConfigService *config)
{
    config->ReadFromConfigFile(&mMysql);
}
