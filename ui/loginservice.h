#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "communication/client.h"
#include "messageservice.h"

namespace Ui {
class LoginService;
}

class LoginService : public QWidget
{
    Q_OBJECT

public:
    explicit LoginService(QWidget *parent = 0);
    ~LoginService();
    void closeEvent(QCloseEvent *event);
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
#if 0
    void ClickedSendButtonSlot();
    void ClickedRecvButtonSlot();
#endif
private:
    Ui::LoginService *ui;
    QString mName;
    UserInfo mUserinfo;
    Client *mClient;
    OnlineUser *mHead;
    int mSendId;
    Message mMessage;
    bool mIsClickedUserName;
    MessageService *mMessageService;

};

#endif // LOGINSERVICE_H
