#include "client.h"
#include "logger/logger.h"
#define TAG "Client"

Client::Client()
{

}

Client::~Client()
{

}

//connect client
bool Client::Connected()
{
    bool result = true;
    result = Init();
    if(!result)
    {
        LOG(TAG, "init error ...");
        return false;
    }

    return true;
}

void Client::DisConnected()
{

}

//客户端socket---connect
bool Client::Init()
{
    int result = 0;
    signal(SIGPIPE, RecvSignal);
    mSocketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocketfd < 0)
    {
        LOG(TAG, "socket error ...");
        return false;
    }
    mSaddr.sin_family = AF_INET;
    mSaddr.sin_port = htons(mConfig.port.toUShort());
    mSaddr.sin_addr.s_addr = inet_addr(mConfig.ip.toLatin1().constData());

    result = connect(mSocketfd, (sockaddr *)&mSaddr, (socklen_t)sizeof(mSaddr));
    if(result < 0)
    {
        LOG(TAG, "connect error ...");
        return false;
    }

    return true;
}

void Client::DeInit()
{

}

//write register information to server
bool Client::WriteToServer(const UserInfo &userinfo)
{
    LOG(TAG, "start to write to server ...");
    int result = 0;
    Package *pack = new Package[sizeof(Package) + sizeof(UserInfo)];
    //Package *pack = (Package *)malloc(sizeof(Package) + sizeof(UserInfo));
    pack->type = REGIST;
    pack->length = sizeof(UserInfo);
    memcpy(pack->data, &userinfo, pack->length);
    result = write(mSocketfd, pack, sizeof(Package) + sizeof(UserInfo));
    if(result < 0)
    {
        LOG(TAG, "write register information error ...");
        return false;
    }

    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    //free(pack);
    delete pack;

    return true;
}

//write login information to server
bool Client::WriteToServer(const LoginInfo &inputinfo)
{
    int result = 0;
    //Package *pack = (Package *)malloc(sizeof(Package) + sizeof(LoginInfo));
    Package *pack = new Package[sizeof(Package) + sizeof(LoginInfo)];
    pack->type = LOGIN;
    pack->length = sizeof(LoginInfo);
    memcpy(pack->data, &inputinfo, pack->length);

    result = write(mSocketfd, pack, sizeof(Package) + sizeof(LoginInfo));
    if(result < 0)
    {
        LOG(TAG, "write login information error ...");
        return false;
    }
    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write zero error ...");
        return false;
    }
    //free(pack);
    delete pack;

    return true;
}

//when user login success write login username to server and it want to get login info
bool Client::WriteToServer(const QString &user)
{
    int result = 0;
    QByteArray bytearry = user.toLatin1();
    char *name = bytearry.data();
    Package *pack = (Package *)malloc(sizeof(Package) + strlen(name) + 1);
    pack->length = strlen(name) + 1;
    pack->type = GETLOGININFO;
    memcpy(pack->data, name, pack->length);

    result = write(mSocketfd, pack, sizeof(Package) + pack->length);
    if(result < 0)
    {
        LOG(TAG, "write error in username ...");
        return false;
    }
    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    free(pack);

    return true;
}

//write empty to server want to get online user's list
bool Client::WriteToServer()
{
    int result = 0;
    int send = 1;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(int));
    pack->type = GETONLINECLIENT;
    pack->length = sizeof(int);
    memcpy(pack->data, &send, pack->length);

    result = write(mSocketfd, pack, sizeof(Package) + sizeof(int));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    free(pack);

    return true;
}

//write message to server
bool Client::WriteToServer(const Message &messageinfo)
{
    int result = 0;
    Package *pack = new Package[sizeof(Message) + sizeof(Package)];
    pack->type = MESSAGE;
    pack->length = sizeof(Message);
    memcpy(pack->data, &messageinfo, pack->length);
    result = write(mSocketfd, pack, sizeof(Message) + sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write message to server error ...");
        return false;
    }

    pack->length = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write package head error");
        return false;
    }
    delete pack;

    return true;
}

//read regist/login status is successed or failed
bool Client::ReadFromServer()
{
    int result = 0;
    int status;
    bool regist_status;
    result = read(mSocketfd, &status, sizeof(int));
    if(result < 0)
    {
        LOG(TAG, "read error ...");
        return false;
    }
    LOG(TAG, "read regist status = ", status);
    if(SUCCESSED == status)
    {
        regist_status = true;
    }
    else
    {
        regist_status = false;
    }

    return regist_status;
}

//when user login success should get own's info
bool Client::ReadFromServer(UserInfo *userinfo)
{
    int result = 0;
    result = read(mSocketfd, userinfo, sizeof(UserInfo));
    if(result < 0)
    {
        LOG(TAG, "read login info error ...");
        return false;
    }

    return true;
}

//read message from server
bool Client::ReadFromServer(Message *messageinfo)
{
    int result = 0;
    bool IsStop = false;
    Package pack;

    while(!IsStop)
    {
        result = read(mSocketfd, &pack, sizeof(Package));
        if(result < 0)
        {
            LOG(TAG, "read package head error ...");
            return false;
        }
        if(ZERO == pack.type)
        {
            break;
        }
        else if(MESSAGE == pack.type)
        {
            result = read(mSocketfd, messageinfo, pack.length);
            if(result < 0)
            {
                LOG(TAG, "read message error ...");
                return false;
            }
        }
    }

    LOG(TAG, "recv message = ", messageinfo->message);
    LOG(TAG, "recv message from : ", messageinfo->sendername);
    LOG(TAG, "recv message time = ", messageinfo->time);
    return true;
}

//read online user
OnlineUser *Client::ReadOnlineUser()
{
    int result = 0;
    bool IsStop = false;
    Package pack;
    OnlineUser *pnew;
    OnlineUser *head = NULL;

    while(!IsStop)
    {
        result = read(mSocketfd, &pack, sizeof(Package));
        if(result < 0)
        {
            LOG(TAG, "read error ...");
            return head;
        }
        else if(0 == result)
        {
            IsStop = true;
        }
        if(pack.type == ZERO)
        {
            LOG(TAG, "read over ...");
            IsStop = true;
        }
        else if(pack.type == GETONLINECLIENT)
        {
            pnew = (OnlineUser *)malloc(sizeof(OnlineUser));
            if(NULL == pnew)
            {
                LOG(TAG, "malloc error ...");
                return head;
            }
            int ret = read(mSocketfd, pnew, sizeof(OnlineUser));
            if(ret < 0)
            {
                LOG(TAG, "read error ...");
                return false;
            }
            pnew->next = head;
            head = pnew;
        }
        else
        {
            //LOG(TAG, "read over ...");
            IsStop = true;
        }
    }
    OnlineUser *p = head;
    qDebug() << "\tname\tid";
    while(p)
    {
        qDebug() << "\t" << p->user << "\t" << p->fd;
        p = p->next;
    }

    return head;
}

//signal function
void Client::RecvSignal(int sig)
{
    LOG(TAG, "recv signal = ", sig);
}

//get config pointer from ManagerService class
void Client::GetConfigFromManager(ConfigService *config)
{
    config->ReadFromConfigFile(&mConfig);
}

OnlineUser *Client::GetOnlineHead()
{
    return mHead;
}
