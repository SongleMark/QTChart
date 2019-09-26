#ifndef SERVER_H
#define SERVER_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "config/configservice.h"
#include "storage/storageservice.h"

struct ClientNode
{
   int fd;
   sockaddr_in caddr;
   ClientNode *next;
};

class Server
{
public:
    explicit Server(bool isstop = false, ClientNode *head = NULL, OnlineUser *uhead = NULL);
    virtual ~Server();
    bool Connected();
    void DisConnected();
    bool Init();
    void DeInit();
    bool SelectClient();
    //write
    bool WriteToClient(int flag);
    bool WriteToClient(const UserInfo &userinfo);
    bool WriteToClient();
    bool WriteToClient(const Message &messageinfo);
    //read
    bool ReadFromClient();
    static void *SelectThread(void *task);
    static void RecvSignal(int sig);
    void GetConfigFromManager(ConfigService *config);
    int GetIsServer();
    void GetStorageFromManager(StorageService *storage);
    void CreateOnlineLinkTable(int fd, const char *username);
    void DeleteUnlineUser(int fd);

private:
    int mSocketfd;
    int mMaxfd;
    int mAcceptfd;
    int mTempfd;
    sockaddr_in mSaddr;
    sockaddr_in mCaddr;
    fd_set mSet;
    fd_set mReadset;
    Communication mConfig;
    bool mIsStop;
    UserInfo mUser;
    LoginInfo mLogin;
    ClientNode *mHead;
    ClientNode *mNode;
    int mIsServer;
    pthread_t mPthread;
    StorageService *mStorage;
    OnlineUser *mUhead;
    Message mMessage;
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

#endif // SERVER_H
