#ifndef CLIENT_H
#define CLIENT_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include "config/configservice.h"

class Client
{
public:
    explicit Client();
    virtual ~Client();
    bool Connected();
    void DisConnected();
    bool Init();
    void DeInit();
    //write
    bool WriteToServer(const UserInfo &userinfo);
    bool WriteToServer(const LoginInfo &inputinfo);
    bool WriteToServer(const QString &user);
    bool WriteToServer();
    bool WriteToServer(const Message &messageinfo);
    //read
    bool ReadFromServer();
    bool ReadFromServer(UserInfo *userinfo);
    bool ReadFromServer(Message *messageinfo);
    OnlineUser *ReadOnlineUser();
    static void RecvSignal(int sig);
    void GetConfigFromManager(ConfigService *config);
    OnlineUser *GetOnlineHead();

private:
    int mSocketfd;
    sockaddr_in mSaddr;
    Communication mConfig;
    OnlineUser *mHead;
    enum
    {
        ZERO,
        REGIST,
        LOGIN,
        GETLOGININFO,
        GETONLINECLIENT,
        MESSAGE
    };
    enum
    {
        FAILED,
        SUCCESSED
    };
};

#endif // CLIENT_H
