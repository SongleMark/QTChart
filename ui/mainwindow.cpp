#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger/logger.h"
#include <QMessageBox>
#include <QKeyEvent>

#define TAG "MainWindow"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mListener->CloseEvent();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        ClickedLoginSlot();
        break;
    case Qt::Key_F1:
        showFullScreen();
        break;
    case Qt::Key_F2:
        showNormal();
        break;
    default:
        break;
    }
}

//初始化主界面
void MainWindow::Init()
{
    ui->setupUi(this);
    move(400, 250);
    SetInputFormat();
    setWindowTitle(tr("局域网聊天系统V1.0.0"));
    setWindowIcon(QIcon(MAINICON));
    QAction *nameaction = new QAction(this);
    QAction *passwordaction = new QAction(this);
    nameaction->setIcon(QIcon(NAMEICON));
    passwordaction->setIcon(QIcon(PASSWORDICON));
    ui->lineEditname->setPlaceholderText(tr("请输入用户名"));
    ui->lineEditpassword->setPlaceholderText(tr("请输入密码(数字字母小数点)"));
    ui->lineEditname->addAction(nameaction, QLineEdit::LeadingPosition);
    ui->lineEditpassword->addAction(passwordaction, QLineEdit::LeadingPosition);
    ui->lineEditpassword->setEchoMode(QLineEdit::Password);

    connect(ui->pushButtonregister, SIGNAL(clicked(bool)), this, SLOT(ClickedRegistSlot()));
    connect(ui->pushButtonlogin, SIGNAL(clicked(bool)), this, SLOT(ClickedLoginSlot()));
}

//设置输入格式
void MainWindow::SetInputFormat()
{
    QRegExp preg(tr("[0-9a-zA-Z.]+$"));
    QRegExpValidator *validator = new QRegExpValidator(this);
    validator->setRegExp(preg);
    ui->lineEditpassword->setValidator(validator);
}

//从manager类获取注册登录/client类指针
void MainWindow::GetUiPointFromManager(RegisterService *regist, LoginService *login, Client *client)
{
    mRegist = regist;
    mLogin = login;
    mClient = client;
}

//listen->manager
void MainWindow::AddListener(Listener *listen)
{
    mListener = listen;
}

//get user input
bool MainWindow::GetLoginInfo()
{
    QByteArray aname = ui->lineEditname->text().toLatin1();
    QByteArray apassword = ui->lineEditpassword->text().toLatin1();
    char *name = aname.data();
    char *password = apassword.data();
    strcpy(mUserInput.username, name);
    strcpy(mUserInput.password, password);
    LOG(TAG, "user input name = ", mUserInput.username);
    LOG(TAG, "user input password = ", mUserInput.password);
    //mUserInput.username = ui->lineEditname->text();
    //mUserInput.password = ui->lineEditpassword->text();
#if 0
    if(mUserInput.username.isEmpty())
    {
        QMessageBox::warning(this, tr("错误"), tr("用户名不能为空"));
        return false;
    }
    if(mUserInput.password.isEmpty())
    {
        QMessageBox::warning(this, tr("错误"), tr("密码不能为空"));
        return false;
    }
#endif
    if(0 == strlen(mUserInput.username))
    {
        QMessageBox::warning(this, tr("错误"), tr("用户名不能为空"));
        return false;
    }
    if(0 == strlen(mUserInput.password))
    {
        QMessageBox::warning(this, tr("错误"), tr("密码不能为空"));
        return false;
    }
    return true;
}

//登录按钮槽函数
void MainWindow::ClickedLoginSlot()
{
    bool result = true;
    result = GetLoginInfo();
    if(result)
    {
        mLogin->GetUserName(mUserInput.username);
        if(mClient->WriteToServer(mUserInput))
        {
            bool ret = mClient->ReadFromServer();
            LOG(TAG, "login ret = ", ret);
            if(ret) //if括号内的内容尽量不要直接调用函数，否则容易造成卡死
            {
                bool retw = mClient->WriteToServer(mUserInput.username);
                if(retw)
                {
                    mClient->ReadFromServer(&mUserinfo);
                    mLogin->GetUserInfo(mUserinfo);
                }
                mLogin->show();
            }
            else
            {
                QMessageBox::information(nullptr, tr("失败"), tr("登录失败"));
                ui->lineEditname->clear();
                ui->lineEditpassword->clear();
            }
        }
        else
        {
            QMessageBox::information(nullptr, tr("失败"), tr("登录失败"));
            ui->lineEditname->clear();
            ui->lineEditpassword->clear();
        }
    }
    else
    {
        return ;
    }
}

//注册按钮槽函数
void MainWindow::ClickedRegistSlot()
{
    mRegist->show();
}
