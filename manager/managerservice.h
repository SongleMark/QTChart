#ifndef MANAGERSERVICE_H
#define MANAGERSERVICE_H
#include "ui/mainwindow.h"
#include "controller/controllerservice.h"
#include <QThread>

class ManagerService : public Listener
{
public:
    explicit ManagerService();
    virtual ~ManagerService();
    void CloseEvent();

private:
    MainWindow *mMainWindow;
    LoginService *mLogin;
    RegisterService *mRegist;
    StorageService *mStorage;
    ConfigService *mConfig;
    Client *mClient;
    Server *mServer;
    ControllerService *mControl;
    enum
    {
        NOTSERVER,
        ISSERVER
    };

};

#endif // MANAGERSERVICE_H
