#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "registerservice.h"
#include "loginservice.h"
#include "communication/client.h"

#define NAMEICON     ":/image/resources/name.jpeg"
#define PASSWORDICON ":/image/resources/password.jpg"
#define MAINICON     ":/image/resources/oicq.jpg"

class Listener
{
public:
    virtual void CloseEvent() = 0;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void Init();
    void SetInputFormat();
    void GetUiPointFromManager(RegisterService *regist, LoginService *login, Client *client);
    void AddListener(Listener *listen);
    bool GetLoginInfo();

private slots:
    void ClickedRegistSlot();
    void ClickedLoginSlot();

private:
    Ui::MainWindow *ui;
    RegisterService *mRegist;
    LoginService *mLogin;
    Listener *mListener;
    Client *mClient;
    LoginInfo mUserInput;
    UserInfo mUserinfo;
};

#endif // MAINWINDOW_H
