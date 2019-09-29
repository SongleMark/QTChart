#include "loginservice.h"
#include "ui_loginservice.h"
#include "logger/logger.h"
#include <QTimer>
#include <QMessageBox>

#define TAG "LoginService"

LoginService::LoginService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginService)
{

}

LoginService::~LoginService()
{
    delete ui;
    delete mMessageService;
}

void LoginService::closeEvent(QCloseEvent *event)
{

}

//init main menu
void LoginService::Init()
{
    ui->setupUi(this);
    move(400, 150);
    InitTime();
    mIsClickedUserName = false;
    mMessageService = new MessageService();
    //ui->textEditSend->setPlaceholderText(tr("请输入要发送的信息，点击左边用户列表的用户名即可给指定的用户发信息"));
    connect(ui->pushButtonGetClient, SIGNAL(clicked(bool)), this, SLOT(ClickedGetOnlineUserSlot()));
    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(ClickedOnlineListSlot(int,int)));
    //connect(ui->pushButtonRecv, SIGNAL(clicked(bool)), this, SLOT(ClickedRecvButtonSlot()));
    //connect(ui->pushButtonSend, SIGNAL(clicked(bool)), this, SLOT(ClickedSendButtonSlot()));
}

//init time
void LoginService::InitTime()
{
    ui->textEditCurrent->setReadOnly(true);
    ui->textEditStartTime->setReadOnly(true);
    QString time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss");
    ui->textEditStartTime->setText(time);
    QThread *thread = new QThread(this);
    QTimer *timer = new QTimer();
    thread->start();
    timer->setInterval(1000);
    timer->moveToThread(thread);

    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    connect(thread, SIGNAL(started()), timer, SLOT(start()));
}

//get login user's information
void LoginService::GetUserInfo(UserInfo userinfo)
{
    mUserinfo = userinfo;
    ui->labeluser->setText(mUserinfo.user);
    ui->labelbirthday->setText(mUserinfo.birthday);
    QString sex = QString::fromLocal8Bit(mUserinfo.sex);
    ui->labelsex->setText(sex);
    ui->textEdit->setText(mUserinfo.signature);
    QString address = QString::fromLocal8Bit(mUserinfo.address);
    ui->labeladdress->setText(address);

    QImage image;
    if(!image.load(QString::fromLocal8Bit(mUserinfo.imagepath)))
    {
        LOG(TAG, "load image error ...");
        return ;
    }
    QSize size = ui->labelimage->size();
    QImage newimage = image.scaled(size, Qt::IgnoreAspectRatio);
    ui->labelimage->setPixmap(QPixmap::fromImage(newimage));
}

//get login username from mainwindow
void LoginService::GetUserName(QString name)
{
    mName = name;
    setWindowTitle(tr("欢迎登录, %1").arg(mName));
}

//get client pointer from manager
void LoginService::GetClientFromManager(Client *client)
{
    mClient = client;
}

//get the number of online user
int LoginService::GetOnlineUserNum(OnlineUser *head)
{
    int len = 0;
    while(NULL != head)
    {
        len ++;
        head = head->next;
    }

    return len;
}

//set tablewidget show online user list
void LoginService::SetOnlineUserTable(OnlineUser *head)
{
    //OnlineUser *head = mClient->GetOnlineHead();
    int len  = GetOnlineUserNum(head);
    ui->tableWidget->setRowCount(len);
    ui->tableWidget->setColumnCount(2);
    QStringList header;
    header << QStringLiteral("用户名") << QStringLiteral("ID") ;
    ui->tableWidget->setHorizontalHeaderLabels(header);
    //设置tablewidget的行自动填充
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int i = 0;
    OnlineUser *p = head;
    while(p)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(p->user));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(p->fd, 10)));
        i++;
        p = p->next;
    }
}

//update current time
void LoginService::UpdateTime()
{
    QString time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss");
    ui->textEditCurrent->setText(time);
}

//clicked get online button
void LoginService::ClickedGetOnlineUserSlot()
{   
    OnlineUser *head = NULL;
    if(mClient->WriteToServer())
    {
        head = mClient->ReadOnlineUser();
        SetOnlineUserTable(head);
    }
}

//clocked online username
void LoginService::ClickedOnlineListSlot(int row, int column)
{
    QString name;
    QString id;
    if(0 == column)
    {
        name = ui->tableWidget->item(row, column)->text();
        id = ui->tableWidget->item(row, column + 1)->text();
    }
    if(1 == column)
    {
        name = ui->tableWidget->item(row, column - 1)->text();
        id = ui->tableWidget->item(row, column)->text();
    }
    mIsClickedUserName = true;
    QString str = QString("当前消息发送对象：%1").arg(name);
    LOG(TAG, "str = ", str);
    ui->labelSendName->setText(str);
    mMessage.receiverid = id.toInt(); //receiver's id
    strcpy(mMessage.receivername, name.toStdString().c_str());//receiver's name
    strcpy(mMessage.sendername, mUserinfo.user);//sender's name

    mMessageService->GetTitleFromLogin(QString("消息将发送给:%1").arg(name));
    mMessageService->GetMessageFromLogin(mMessage);
    mMessageService->GetClientFromLogin(mClient);
    mMessageService->show();
}

#if 0
//clicked send messages
void LoginService::ClickedSendButtonSlot()
{
    if(!mIsClickedUserName)
    {
        QMessageBox::information(nullptr, tr("未选择发送对象"), tr("点击在线用户用户名或id即可发送"));
        return ;
    }
    QString message = ui->textEditSend->toPlainText();
    const char *mes = message.toStdString().c_str();
    strcpy(mMessage.message, mes);
    QDateTime ctime = QDateTime::currentDateTime();
    QByteArray time = ctime.toString("yyyy-MM-dd|hh:mm:ss").toLatin1();
    char *mestime = time.data();
    strcpy(mMessage.time, mestime);
    LOG(TAG, "user send time = ", mMessage.time);
    if(mClient->WriteToServer(mMessage))
    {
        QString str = QString("[%1]:[%2]:\n%3").arg(ctime.toString()).arg(QString::fromLocal8Bit(mMessage.sendername))
                                                 .arg(QString::fromLocal8Bit(mMessage.message));
        //ui->textEditMessage->setText(str);
        LOG(TAG, "send ", str);
    }
}

//recv message from
void LoginService::ClickedRecvButtonSlot()
{
    //mThread->RecvMessage(1);
    Message message;
    if(mClient->ReadFromServer(&message))
    {
        if(strlen(message.message) == 0 && strlen(message.receivername) == 0 && strlen(message.sendername) == 0)
        {
            QMessageBox::information(nullptr, tr("没有信息"), tr("当前无人发送信息"));
            return ;
        }
        QString mes = QString::fromLocal8Bit(message.message);
        QString messendname = QString::fromLocal8Bit(message.sendername);
        QString mestime = QString::fromLocal8Bit(message.time);
        QString show = QString("[%1]:[%2]:\n%3").arg(mestime).arg(messendname).arg(mes);
        ui->textEditMessage->append(show);
    }
}
#endif
