#include "managerservice.h"
#include "logger/logger.h"
#include <QMessageBox>
#define TAG "ManagerService"

ManagerService::ManagerService()
{
    mMainWindow = new MainWindow();
    mRegist = new RegisterService();
    mLogin = new LoginService();
    //[control]
    mControl = new ControllerService();
    if(!mControl->InitAll())
    {
        LOG(TAG, "init controller error ...");
        return ;
    }
    mConfig = mControl->GetConfig();
    mClient = mControl->GetClient();
    //[register]
    mRegist->GetConfigFromManager(mConfig);
    mRegist->Init();   
    if(mClient->Connected())
    {
        mRegist->GetClientFromManager(mClient);
    }
    else
    {
        QMessageBox::warning(nullptr, QObject::tr("连接失败"), QObject::tr("连接服务器失败"));
        return ;
    }
    //[login]
    mLogin->GetClientFromManager(mClient);
    mLogin->Init();   
    //[MainWindow]
    mMainWindow->AddListener(this);
    mMainWindow->GetUiPointFromManager(mRegist, mLogin, mClient);
    mMainWindow->Init();
    mMainWindow->show();
}

ManagerService::~ManagerService()
{
    delete mMainWindow;
    delete mRegist;
    delete mStorage;
    delete mLogin;
    delete mConfig;
    delete mServer;
    delete mClient;
}

//当点击主界面关闭按钮
void ManagerService::CloseEvent()
{
    mControl->StopThread();
}

