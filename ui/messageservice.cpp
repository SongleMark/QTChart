#include "messageservice.h"
#include "ui_messageservice.h"
#include "logger/logger.h"
#include <QColorDialog>
#include <QMessageBox>

#define TAG "MessageService"

MessageService::MessageService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageService)
{
    ui->setupUi(this);
    move(400, 200);
    ui->textEditSend->setPlaceholderText(tr("请输入要发送的消息"));

    connect(ui->ButtonBlod, SIGNAL(clicked(bool)), this, SLOT(ButtonBlodClickedSlot(bool)));
    connect(ui->ButtonClear, SIGNAL(clicked(bool)), this, SLOT(ButtonClearClickedSlot()));
    connect(ui->ButtonColor, SIGNAL(clicked(bool)), this, SLOT(ButtonColorClickedSlot()));
    connect(ui->ButtonFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(ChangeFontSlot(QFont)));
    connect(ui->BUttonFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeFontSizeSlot(QString)));
    connect(ui->ButtonItalic, SIGNAL(clicked(bool)), this, SLOT(ButtonItalicClickedSlot(bool)));
    connect(ui->ButtonSave, SIGNAL(clicked(bool)), this, SLOT(ButtonSaveClickedSlot()));
    connect(ui->pushButtonSend, SIGNAL(clicked(bool)), this, SLOT(ButtonSendClickedSlot()));
    connect(ui->ButtonUnder, SIGNAL(clicked(bool)), this, SLOT(ButtonUnderClickedSlot(bool)));
    connect(ui->ButtonSend, SIGNAL(clicked(bool)), this, SLOT(ButtonSendFileClickedSlot()));
    connect(ui->pushButtonUpdate, SIGNAL(clicked(bool)), this, SLOT(ButtonUpdateOnlineUserListSlot()));
    connect(ui->pushButtonRecv, SIGNAL(clicked(bool)), this, SLOT(ButtonRecvMessageSlot()));
}

MessageService::~MessageService()
{
    delete ui;
}

void MessageService::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        ButtonSendClickedSlot();
        break;
    default:
        break;
    }
}

void MessageService::GetTitleFromLogin(QString title)
{
    setWindowTitle(title);
}

void MessageService::GetMessageFromLogin(Message message)
{
    mMessage = message;
}

void MessageService::GetClientFromLogin(Client *client)
{
    mClient = client;
}

void MessageService::SetOnlineTable(OnlineUser *head)
{
    //OnlineUser *head = mClient->GetOnlineHead();
    int len  = GetOnlineUserNum(head);
    ui->tableWidgetUser->setRowCount(len);
    ui->tableWidgetUser->setColumnCount(2);
    QStringList header;
    header << QStringLiteral("用户名") << QStringLiteral("ID") ;
    ui->tableWidgetUser->setHorizontalHeaderLabels(header);
    //设置tablewidget的行自动填充
    ui->tableWidgetUser->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int i = 0;
    OnlineUser *p = head;
    while(p)
    {
        ui->tableWidgetUser->setItem(i, 0, new QTableWidgetItem(p->user));
        ui->tableWidgetUser->setItem(i, 1, new QTableWidgetItem(QString::number(p->fd, 10)));
        i++;
        p = p->next;
    }
}

int MessageService::GetOnlineUserNum(OnlineUser *head)
{
    int len = 0;
    while(NULL != head)
    {
        len ++;
        head = head->next;
    }

    return len;
}

void MessageService::ChangeFontSlot(const QFont &font)
{
    ui->textEditMessage->setCurrentFont(font);
    ui->textEditSend->setCurrentFont(font);
    ui->textEditMessage->setFocus();
    ui->textEditSend->setFocus();
}

void MessageService::ChangeFontSizeSlot(const QString &size)
{
    ui->textEditMessage->setFontPointSize(size.toDouble());
    ui->textEditSend->setFontPointSize(size.toDouble());
    ui->textEditMessage->setFocus();
    ui->textEditSend->setFocus();
}

void MessageService::ButtonBlodClickedSlot(bool check)
{
    if(check)
    {
        ui->textEditMessage->setFixedWidth(QFont::Bold);
        ui->textEditSend->setFontWeight(QFont::Bold);
    }
    else
    {
        ui->textEditMessage->setFontWeight(QFont::Normal);
        ui->textEditSend->setFontWeight(QFont::Normal);
    }
    ui->textEditMessage->setFocus();
    ui->textEditSend->setFocus();
}

void MessageService::ButtonItalicClickedSlot(bool check)
{
    ui->textEditMessage->setFontItalic(check);
    ui->textEditSend->setFontItalic(check);
    ui->textEditMessage->setFocus();
    ui->textEditSend->setFocus();
}

void MessageService::ButtonUnderClickedSlot(bool check)
{
    ui->textEditMessage->setFontUnderline(check);
    ui->textEditSend->setFontUnderline(check);
    ui->textEditMessage->setFocus();
    ui->textEditSend->setFocus();
}

void MessageService::ButtonSaveClickedSlot()
{

}

void MessageService::ButtonSendClickedSlot()
{
    QString message = ui->textEditSend->toPlainText();
    const char *mes = message.toStdString().c_str();
    strcpy(mMessage.message, mes);

    QDateTime ctime = QDateTime::currentDateTime();
    QByteArray time = ctime.toString("yyyy-MM-dd|hh:mm:ss").toLatin1();
    char *mestime = time.data();
    strcpy(mMessage.time, mestime);

    if(mClient->WriteToServer(mMessage))
    {
        QString str = QString("[%1]:[%2]:\n%3").arg(ctime.toString()).arg(QString::fromLocal8Bit(mMessage.sendername))
                                                 .arg(QString::fromLocal8Bit(mMessage.message));
        ui->textEditMessage->append(str);
        LOG(TAG, "send ", str);
    }
}

void MessageService::ButtonClearClickedSlot()
{
    ui->textEditMessage->clear();
}

void MessageService::ButtonColorClickedSlot()
{
    QColor color = QColorDialog::getColor(color, this);
    if(color.isValid())
    {
        ui->textEditMessage->setTextColor(color);
        ui->textEditSend->setTextColor(color);
        ui->textEditMessage->setFocus();
        ui->textEditSend->setFocus();
    }
}

void MessageService::ButtonSendFileClickedSlot()
{

}

void MessageService::ButtonUpdateOnlineUserListSlot()
{
    OnlineUser *head;
    if(mClient->WriteToServer())
    {
        head = mClient->ReadOnlineUser();
        SetOnlineTable(head);
    }
}

void MessageService::ButtonRecvMessageSlot()
{
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
