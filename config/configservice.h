#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H
#include <QSettings>

//mysql information
struct Storage
{
    QString database;
    QString username;
    QString hostname;
    QString password;
    int port;
    QString table;
};

//client's and server's some config information
struct Communication
{
    QString ip;
    QString port;
    QString isserver;
};

//user register information
struct UserInfo
{
    char name[32];     //姓名
    char user[32];     //用户名
    char password[32]; //密码
    char sex[8];      //性别
    char address[40];  //户籍地
    char emil[32];     //邮箱
    char imagepath[60];//头像地址
    char signature[66];//个性签名
    int64_t tel;      //电话
    int64_t Id;       //身份证号
    char birthday[66]; //出生日期
};

//package head
struct Package
{
    int type;
    int length;
    char data[0];
};

//user input UserName and PassWord
struct LoginInfo
{
    char username[32];
    char password[32];
};

//online user linktable
struct OnlineUser
{
    int fd;
    char user[32];
    OnlineUser *next;
};

//Message
struct Message
{
    char time[40];
    int receiverid;
    int senderid;
    char receivername[32];
    char sendername[32];
    char message[1024];
};

class ConfigService
{
public:
    static ConfigService *mInstance;
    static ConfigService *GetInstance();
    virtual ~ConfigService();
    void ReadFromConfigFile(Storage *storage);
    void ReadFromConfigFile(Communication *storage);
    QString ReadFromConfigFile();

private:
    explicit ConfigService();
};

#endif // CONFIGSERVICE_H
