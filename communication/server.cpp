#include "server.h"
#include "logger/logger.h"
#define TAG "Server"

Server::Server(bool isstop, ClientNode *head, OnlineUser *uhead):
    mIsStop(isstop),
    mHead(head),
    mUhead(uhead)
{

}

Server::~Server()
{
}

//connect
bool Server::Connected()
{
    void *exit_p;
    bool result = Init();
    if(result)
    {
        if(0 != pthread_create(&mPthread, NULL, SelectThread, this))
        {
            LOG(TAG, "create thread error ...");
            return false;
        }
        if(0 != pthread_join(mPthread, &exit_p))
        {
            LOG(TAG, "join thread error ...");
            return false;
        }
    }

    return true;
}

void Server::DisConnected()
{

}

//socket->bind->listen
bool Server::Init()
{
    int result = 0;
    signal(SIGPIPE, RecvSignal);
    LOG(TAG, "socket ...");
    mSocketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocketfd < 0)
    {
        LOG(TAG, "socket error ...");
        return false;
    }
    mSaddr.sin_family = AF_INET;
    mSaddr.sin_port = htons(mConfig.port.toUShort());
    mSaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t on = 1;
    result = setsockopt(mSocketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(result < 0)
    {
        LOG(TAG, "setsockopt error ...");
        return false;
    }

    LOG(TAG, "bind ...");
    result = bind(mSocketfd, (sockaddr *)&mSaddr, (socklen_t)sizeof(mSaddr));
    if(result < 0)
    {
        LOG(TAG, "bind error ...");
        return false;
    }

    LOG(TAG, "listen ...");
    result = listen(mSocketfd, 20);
    if(result < 0)
    {
        LOG(TAG, "listen error ...");
        return false;
    }

    return true;
}

void Server::DeInit()
{
    mIsStop = true;
}

//send regist/login status is successed or failed
bool Server::WriteToClient(int flag)
{
    int result = 0;
    result = write(mTempfd, &flag, sizeof(int));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    LOG(TAG, "regist/login successed ...");

    return true;
}

//send userinfo to user who is login successed
bool Server::WriteToClient(const UserInfo &userinfo)
{
    int result = 0;
    result = write(mTempfd, &userinfo, sizeof(UserInfo));
    if(result < 0)
    {
        LOG(TAG, "write login info error ...");
        return false;
    }
    LOG(TAG, "write userinfo success ...");

    return true;
}

//send online list to client
bool Server::WriteToClient()
{
    int result = 0;
    OnlineUser *p = mUhead;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(OnlineUser));
    pack->type = GETONLINECLIENT;
    pack->length = sizeof(OnlineUser);

    while(p)
    {
        memcpy(pack->data, p, pack->length);
        result = write(mTempfd, pack, sizeof(Package) + sizeof(OnlineUser));
        if(result < 0)
        {
            LOG(TAG, "write error ...");
            return false;
        }
        p = p->next;
    }
    pack->type = ZERO;
    result = write(mTempfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write error ...");
        return false;
    }
    LOG(TAG, "write user list success ...");
    OnlineUser *pshow = mUhead;
    qDebug() << "server:\tname\tId";
    while(pshow)
    {
        qDebug() << "server:\t" << pshow->user << "\t" << pshow->fd;
        pshow = pshow->next;
    }

    return true;
}

//write message to client
bool Server::WriteToClient(const Message &messageinfo)
{
    int result = 0;
    Package *pack = new Package[sizeof(Message) + sizeof(Package)];
    pack->type = MESSAGE;
    pack->length = sizeof(Message);
    memcpy(pack->data, &messageinfo, pack->length);
    result = write(messageinfo.receiverid, pack, sizeof(Package) + sizeof(Message));
    if(result < 0)
    {
        LOG(TAG, "write message error ...");
        return false;
    }
    pack->type = ZERO;
    result = write(messageinfo.receiverid, pack, sizeof(Package));
    if(result < 0)
    {
        LOG(TAG, "write pack head error ...");
        return false;
    }
    delete pack;

    return true;
}

bool Server::ReadFromClient()
{
    return true;
}

//select client
bool Server::SelectClient()
{
    FD_ZERO(&mSet);
    FD_SET(mSocketfd, &mSet);
    mMaxfd = mSocketfd;

    while(!mIsStop)
    {
        mReadset = mSet;
        int result = 0;
        LOG(TAG, "select ...");
        result = select(mMaxfd + 1, &mReadset, NULL, NULL, NULL);
        if(result < 0)
        {
            LOG(TAG, "select error ...");
            return false;
        }

        if(FD_ISSET(mSocketfd, &mReadset))
        {
            socklen_t len = sizeof(mCaddr);
            mAcceptfd = accept(mSocketfd, (sockaddr *)&mSaddr, &len);
            if(mAcceptfd < 0)
            {
                LOG(TAG, "accept error ...");
                return false;
            }
            LOG(TAG, "Client: ", inet_ntoa(mSaddr.sin_addr));
            FD_SET(mAcceptfd, &mSet);
            if(mMaxfd < mAcceptfd)
            {
                mMaxfd = mAcceptfd;
            }

            //ClientNode *pnode = new ClientNode[sizeof(ClientNode)];
            mNode = (ClientNode *)malloc(sizeof(ClientNode));
            mNode->fd = mAcceptfd;
            mNode->caddr = mCaddr;
            mNode->next = mHead;
            mHead = mNode;
        }

        for(mNode = mHead; mNode != NULL; mNode = mNode->next)
        {
            mTempfd = mNode->fd;
            if(!FD_ISSET(mTempfd, &mReadset))
            {
                continue;
            }

            Package pack;
            result = read(mTempfd, &pack, sizeof(Package));
            if(result < 0)
            {
                LOG(TAG, "read error and TCP broken ...");
                DeleteUnlineUser(mTempfd);
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);
                break;
            }
            else if(0 == result)
            {
                LOG(TAG, "TCP broken ...");
                DeleteUnlineUser(mTempfd);
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);
                break;
            }

            bool IsBreak = false;
            char name[66];
            UserInfo userinfo;
            int online;
            switch(pack.type)
            {
            case REGIST://数据包类型为注册
                result = read(mTempfd, &mUser, pack.length);
                if(result < 0)
                {
                    LOG(TAG, "read user regist info error ...");
                    return false;
                }
                LOG(TAG, "read regist image path = ", mUser.imagepath);
                if(mStorage->WriteToStorage(mUser))
                {
                    WriteToClient(SUCCESSED);
                }
                else
                {
                    WriteToClient(FAILED);
                }
                break;
            case LOGIN://数据包类型为登录
                result = read(mTempfd, &mLogin, pack.length);
                if(result < 0)
                {
                    LOG(TAG, "read login info error ...");
                    return false;
                }
                LOG(TAG, "read login name = ", mLogin.username);
                LOG(TAG, "read login password = ", mLogin.password);
                if(mStorage->ReadFromStorage(mLogin.username, mLogin.password))
                {
                    CreateOnlineLinkTable(mTempfd, mLogin.username);
                    WriteToClient(SUCCESSED);
                }
                else
                {
                    WriteToClient(FAILED);
                }
                break;
            case GETLOGININFO://包头表示获取登录成功用户自身信息
                result = read(mTempfd, name, pack.length);
                if(result < 0)
                {
                    LOG(TAG, "read user name error ...");
                    return false;
                }
                LOG(TAG, "read username = ", name);
                userinfo = mStorage->ReadFromStorage(name);
                WriteToClient(userinfo);
                break;
            case GETONLINECLIENT://包头表示获取在线用户
                result = read(mTempfd, &online, sizeof(int));
                if(result < 0)
                {
                    LOG(TAG, "read user name error ...");
                    return false;
                }
                WriteToClient();
                break;               
            case MESSAGE://包头表示消息
                result = read(mTempfd, &mMessage, pack.length);
                if(result < 0)
                {
                    LOG(TAG, "read message error ...");
                    return false;
                }
                mMessage.senderid = mTempfd;
                if(WriteToClient(mMessage))
                {
                    LOG(TAG, "write message to client success ...");
                }
                break;
            case ZERO:
            default:
                IsBreak = true;
                break;
            }
            if(IsBreak)
            {
                break;
            }

        }
    }

    return true;
}

//recv signal function
void Server::RecvSignal(int sig)
{
    LOG(TAG, "recv signal = ", sig);
}

//select client thread
void *Server::SelectThread(void *task)
{
    bool result = true;
    Server *thread = (Server *)task;
    result = thread->SelectClient();
    if(!result)
    {
        LOG(TAG, "select client error ...");
        return NULL;
    }
}

//get config file from manager
void Server::GetConfigFromManager(ConfigService *config)
{
    config->ReadFromConfigFile(&mConfig);
    mIsServer = mConfig.isserver.toInt();
    LOG(TAG, "ISSERVER = ", mIsServer);
    LOG(TAG, "ip = ", mConfig.ip.toLatin1().constData());
    LOG(TAG, "port = ", mConfig.port.toInt());
}

//return IsServer
int Server::GetIsServer()
{
    return mIsServer;
}

//get mysql from manager class
void Server::GetStorageFromManager(StorageService *storage)
{
    mStorage = storage;
}

//create online user list
void Server::CreateOnlineLinkTable(int fd, const char *username)
{
    OnlineUser *pnew = (OnlineUser *)malloc(sizeof(OnlineUser));
    OnlineUser *plast = mUhead;;
    pnew->fd = fd;
    strcpy(pnew->user, username);
    pnew->next = NULL;

    if(NULL == mUhead)
    {
        mUhead = pnew;
    }
    else
    {
        plast = mUhead;
        while(NULL != plast->next)
        {
            plast = plast->next;
        }
        plast->next = pnew;
    }

    OnlineUser *p = mUhead;
    qDebug() << "server:\tname\tId";
    while(p)
    {
        qDebug() << "server:\t" << p->user << "\t" << p->fd;
        p = p->next;
    }
}

void Server::DeleteUnlineUser(int fd)
{
    OnlineUser *p = mUhead;
    OnlineUser *pre = mUhead;
    while(NULL != p)
    {
        if(fd == p->fd)
        {
            break;
        }
        p = p->next;
    }

    if(p == NULL)
    {
        LOG(TAG, "something error can't think about ...");
        return ;
    }
    if(p == mUhead)
    {
        mUhead = mUhead->next;
    }
    else
    {
        pre = mUhead;
        while(pre->next != p)
        {
            pre = pre->next;
        }
        pre->next = p->next;
    }
}
