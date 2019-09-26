#ifndef STORAGESERVICE_H
#define STORAGESERVICE_H
#include "config/configservice.h"
#include <QSqlDatabase>
#include <QSqlQuery>

class StorageService
{
public:
    static StorageService *mInstance;
    static StorageService *GetInstance();
    virtual ~StorageService();
    bool Connected();
    void DisConnected();
    bool WriteToStorage(const UserInfo &info);
    bool ReadFromStorage(const char *user, const char *password);
    UserInfo ReadFromStorage(const QString &username);
    void GetConfigFromManager(ConfigService *config);

private:
    explicit StorageService();
    QSqlDatabase mDb;
    QSqlQuery *mQuery;
    ConfigService *mConfig;
    Storage mMysql;
};

#endif // STORAGESERVICE_H
