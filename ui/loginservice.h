#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "communication/client.h"

class LoginService ;

class MessageThread : public QThread
{
    Q_OBJECT

public:
    explicit MessageThread(Client *client, bool IsStop = false, int run = 0);
    virtual ~MessageThread();
    void run();
    void RecvMessage(int run);

private:
    Client *mClietn;
    bool mIsStop;
    Message mMessage;
    int mIsRun;
    QMutex mMutex;
    QWaitCondition mWaitCondition;

signals:
    void RecvMessage(Message message);
};

namespace Ui {
class LoginService;
}

class LoginService : public QWidget
{
    Q_OBJECT

public:
    explicit LoginService(QWidget *parent = 0);
    ~LoginService();
    void Init();
    void GetUserName(QString name);
    void InitTime();
    void GetUserInfo(UserInfo userinfo);
    void GetClientFromManager(Client *client);
    int  GetOnlineUserNum(OnlineUser *head);
    void SetOnlineUserTable(OnlineUser *head);

private slots:
    void UpdateTime();
    void ClickedGetOnlineUserSlot();
    void ClickedOnlineListSlot(int row, int column);
    void ClickedSendButtonSlot();
    void RecvMessage(Message message);
    void ClickedRecvButtonSlot();

private:
    Ui::LoginService *ui;
    QString mName;
    UserInfo mUserinfo;
    Client *mClient;
    OnlineUser *mHead;
    int mSendId;
    Message mMessage;
    MessageThread *mThread;

};

#endif // LOGINSERVICE_H
