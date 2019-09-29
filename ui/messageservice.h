#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include <QWidget>
#include <QKeyEvent>
#include <QThread>
#include "communication/client.h"

namespace Ui {
class MessageService;
}

class MessageService : public QWidget
{
    Q_OBJECT

public:
    explicit MessageService(QWidget *parent = 0);
    ~MessageService();
    void keyPressEvent(QKeyEvent *event);
    void GetTitleFromLogin(QString title);
    void GetMessageFromLogin(Message message);
    void GetClientFromLogin(Client *client);
    void SetOnlineTable(OnlineUser *head);
    int  GetOnlineUserNum(OnlineUser *head);

private slots:
    void ChangeFontSlot(const QFont &font);
    void ChangeFontSizeSlot(const QString &size);
    void ButtonBlodClickedSlot(bool check);
    void ButtonItalicClickedSlot(bool check);
    void ButtonUnderClickedSlot(bool check);
    void ButtonSaveClickedSlot();
    void ButtonSendClickedSlot();
    void ButtonClearClickedSlot();
    void ButtonColorClickedSlot();
    void ButtonSendFileClickedSlot();
    void ButtonUpdateOnlineUserListSlot();
    void ButtonRecvMessageSlot();

private:
    Ui::MessageService *ui;
    Message mMessage;
    Client *mClient;
};

#endif // MESSAGESERVICE_H
