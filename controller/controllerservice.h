#ifndef CONTROLLERSERVICE_H
#define CONTROLLERSERVICE_H
#include "storage/storageservice.h"
#include "communication/client.h"
#include "communication/server.h"
#include <QThread>

class ServerThread : public QThread
{
public:
    explicit ServerThread(Server *server);
    virtual ~ServerThread();
    void run();

private:
    Server *mServer;
};

class ControllerService
{
public:
    explicit ControllerService();
    virtual ~ControllerService();
    bool InitAll();
    void StopThread();
    Client *GetClient();
    ConfigService *GetConfig();

private:
    StorageService *mStorage;
    ConfigService *mConfig;
    Client *mClient;
    Server *mServer;
    ServerThread *mThread;
    enum
    {
        NOTSERVER,
        ISSERVER
    };
};

#endif // CONTROLLERSERVICE_H
