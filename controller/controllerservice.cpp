#include "controllerservice.h"
#include <QMessageBox>
#include "logger/logger.h"
#define TAG "ControllerService"

ServerThread::ServerThread(Server *server):
    mServer(server)
{

}

ServerThread::~ServerThread()
{

}

void ServerThread::run()
{
    bool result = true;
    result = mServer->Connected();
    if(!result)
    {
        LOG(TAG, "connect server error ...");
        return ;
    }
}


ControllerService::ControllerService()
{

}

ControllerService::~ControllerService()
{

}

bool ControllerService::InitAll()
{
    mClient = new Client();
    mServer = new Server();
    //[config]
    mConfig = ConfigService::GetInstance();
    //[server]
    mServer->GetConfigFromManager(mConfig);
    if(ISSERVER == mServer->GetIsServer())
    {
        //[storage]
        mStorage = StorageService::GetInstance();
        mStorage->GetConfigFromManager(mConfig);
        if(!mStorage->Connected())
        {
            QMessageBox::warning(nullptr, QObject::tr("连接失败"), QObject::tr("连接数据库失败"));
            return false;
        }
        mServer->GetStorageFromManager(mStorage);
        //thread
        mThread = new ServerThread(mServer);
        mThread->start();
        mThread->wait(1);
    }
    //[client]
    mClient->GetConfigFromManager(mConfig);
    mClient->Connected();

    return true;
}

void ControllerService::StopThread()
{
    mThread->terminate();
    mThread->wait();
    mThread->deleteLater();
}

ConfigService *ControllerService::GetConfig()
{
    return mConfig;
}

Client *ControllerService::GetClient()
{
    return mClient;
}
