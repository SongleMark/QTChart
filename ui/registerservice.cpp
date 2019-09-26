#include "registerservice.h"
#include "ui_registerservice.h"
#include "logger/logger.h"
#include <QMessageBox>
#include <QDialog>
#include <QFileDialog>
#include <QMouseEvent>

#define TAG "RegisterService"

RegisterService::RegisterService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterService)
{

}

RegisterService::~RegisterService()
{
    delete ui;
}

//注册界面初始化
void RegisterService::Init()
{
    ui->setupUi(this);
    move(400, 200);
    setWindowTitle(tr("注册界面"));
    SetInputFormat();
    ui->labelimage->installEventFilter(this);
    ui->lineEditimagepath->setReadOnly(true);
    ui->lineEditAddress->setPlaceholderText(tr("户籍(xx省xx市xx县/区)"));
    ui->lineEditemil->setPlaceholderText(tr("请输入邮箱"));
    ui->lineEditIdcard->setPlaceholderText(tr("请输入身份证号"));
    ui->lineEditimagepath->setPlaceholderText(tr("请右侧按钮获取头像地址"));
    ui->lineEditname->setPlaceholderText(tr("请输入真实姓名"));
    ui->lineEditpassword->setPlaceholderText(tr("请输入密码(数字字母小数点)"));
    ui->lineEditsure->setPlaceholderText(tr("请再次输入密码确认"));
    ui->lineEdittel->setPlaceholderText(tr("请输入电话号码"));
    ui->lineEditUser->setPlaceholderText(tr("请输入用户名(用户名如已存在则会注册失败)"));
    ui->textEditsignature->setPlaceholderText(tr("请输入个性签名"));

    connect(ui->pushButtonRegist, SIGNAL(clicked(bool)), this, SLOT(ClickedRegistSlot()));
    connect(ui->pushButtonpath, SIGNAL(clicked(bool)), this, SLOT(ClickedChooseImagePathSlot()));
}

//获取用户输入的注册信息
void RegisterService::GetUserInput()
{
    //address
    std::string stdaddress = ui->lineEditAddress->text().toStdString();
    const char *address = stdaddress.c_str();
    strcpy(mUser.address, address);
    //birthday
    QByteArray bytebirthday = ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd|hh:mm:ss").toLatin1();
    char *birthday = bytebirthday.data();
    strcpy(mUser.birthday, birthday);
    //emil
    QByteArray byteemil = ui->lineEditemil->text().toLatin1();
    char *emil = byteemil.data();
    strcpy(mUser.emil, emil);
    //ID
    mUser.Id = ui->lineEditIdcard->text().toULong();
    //Imagepath
    std::string strimage = ui->lineEditimagepath->text().toStdString();
    const char *image = strimage.c_str();
    strcpy(mUser.imagepath, image);
    //name
    std::string stdname = ui->lineEditname->text().toStdString();
    const char *name = stdname.c_str();
    strcpy(mUser.name, name);
    //password
    QByteArray bytepassword = ui->lineEditpassword->text().toLatin1();
    char *password = bytepassword.data();
    strcpy(mUser.password, password);
    //sex
    std::string stdsex = ui->comboBoxsex->currentText().toStdString();
    const char *sex = stdsex.c_str();
    strcpy(mUser.sex, sex);
    //signature
    std::string stdsignature = ui->textEditsignature->toPlainText().toStdString();
    const char *signature = stdsignature.c_str();
    strcpy(mUser.signature, signature);
    //tel
    mUser.tel = ui->lineEdittel->text().toULong();
    //username
    std::string stduser = ui->lineEditUser->text().toStdString();
    const char *user = stduser.c_str();
    strcpy(mUser.user, user);
}

//设置输入的格式
void RegisterService::SetInputFormat()
{
    QRegExp numreg("[0-9]+$");
    QRegExp characterreg("[0-9a-zA-Z.@]+$");
    QRegExpValidator *numValidator = new QRegExpValidator(this);
    QRegExpValidator *characterValidator = new QRegExpValidator(this);
    numValidator->setRegExp(numreg);
    characterValidator->setRegExp(characterreg);

    ui->lineEditemil->setValidator(characterValidator);
    ui->lineEditIdcard->setValidator(numValidator);
    ui->lineEditpassword->setValidator(characterValidator);
    ui->lineEditpassword->setEchoMode(QLineEdit::Password);
    ui->lineEditsure->setValidator(characterValidator);
    ui->lineEditsure->setEchoMode(QLineEdit::Password);
    ui->lineEdittel->setValidator(numValidator);
}

//检查输入的格式
bool RegisterService::CheckInputFormat()
{
    bool cPassword = true;
    bool cIdcard = true;
    bool ctelphone = true;
    bool cUser = true;
    bool result = true;
    if(ui->lineEditpassword->text() == "")
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("密码不能为空"));
        cPassword = false;
    }
    if(ui->lineEditpassword->text() != ui->lineEditsure->text())
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("两次密码输入不一致"));
        ui->lineEditpassword->clear();
        ui->lineEditsure->clear();
        cPassword = false;
    }
    if(ui->lineEditIdcard->text().length() < 11)
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("输入的id不合法"));
        ui->lineEditIdcard->clear();
        cIdcard = false;
    }
    if(ui->lineEdittel->text().length() < 6)
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("输入的电话格式错误"));
        ui->lineEdittel->clear();
        ctelphone = false;
    }
    if(ui->lineEditUser->text() == "")
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("用户名不能为空"));
        cUser = false;
    }

    if(cPassword && cIdcard && ctelphone && cUser)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

//get client pointer from manager class
void RegisterService::GetClientFromManager(Client *client)
{
    mClient = client;
}

//get config pointer from manager class
void RegisterService::GetConfigFromManager(ConfigService *config)
{
    mImagePath = config->ReadFromConfigFile();
    LOG(TAG, "read image path = ", mImagePath);
}

//注册按钮
void RegisterService::ClickedRegistSlot()
{
    bool result = true;
    result = CheckInputFormat();
    if(result)
    {
        GetUserInput();
        if(mClient->WriteToServer(mUser))
        {
            bool ret = mClient->ReadFromServer();
            if(ret)
            {
                QMessageBox::information(nullptr, tr("成功"), tr("注册成功"));
                close();
            }
            else
            {
                QMessageBox::warning(nullptr, tr("失败"), tr("注册失败"));
                close();
            }
        }
        else
        {
            QMessageBox::warning(nullptr, tr("失败"), tr("注册失败"));
            close();
        }
    }
    else
    {
        return ;
    }
}

//user choose image path
void RegisterService::ClickedChooseImagePathSlot()
{
    QString path = QFileDialog::getOpenFileName(this, tr("请选择头像"), mImagePath,
                                                tr("Jpg图片(*.jpg);;bmp图片(*.bmp);;png图片(*.png);;Jpeg图片(*.jpeg)"));
    QImage image;
    if(!image.load(path))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("显示头像错误"));
        LOG(TAG, "load image path failed ...");
        return ;
    }
    ui->lineEditimagepath->setText(path);
    QSize size = ui->labelimage->size();
    QImage newimage = image.scaled(size, Qt::IgnoreAspectRatio);
    ui->labelimage->setPixmap(QPixmap::fromImage(newimage));

}

//cliecked image label to show big size image
bool RegisterService::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->labelimage)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)//当点击了鼠标左键
            {
                cv::Mat mat;
                if(!ui->lineEditimagepath->text().isEmpty())
                {
                    QImage image = ui->labelimage->pixmap()->toImage();
                    if(!image.isNull())
                    {
                        QImage::Format format = image.format();
                        LOG(TAG, "image format = ", format);
                        switch(format)
                        {
                        case QImage::Format_ARGB32:
                        case QImage::Format_RGB32:
                        case QImage::Format_ARGB32_Premultiplied:
                            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
                            break;
                        case QImage::Format_RGB888:
                            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
                            cv::cvtColor(mat, mat, CV_BGR2RGB);
                            break;
                        case QImage::Format_Indexed8:
                            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
                            break;
                        }
                        cv::namedWindow("头像大图", cv::WINDOW_NORMAL);//window_normal is user can resize
                        cv::imshow("头像大图", mat);
                    }
                    else
                    {
                        return true;
                    }
                }
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}


