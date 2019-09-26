#ifndef REGISTERSERVICE_H
#define REGISTERSERVICE_H

#include <QWidget>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>

#include "storage/storageservice.h"
#include "communication/client.h"
#include "config/configservice.h"

namespace Ui {
class RegisterService;
}

class RegisterService : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterService(QWidget *parent = 0);
    ~RegisterService();
    void Init();
    void GetUserInput();
    void SetInputFormat();
    bool CheckInputFormat();
    void GetClientFromManager(Client *client);
    void GetConfigFromManager(ConfigService *config);
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void ClickedRegistSlot();
    void ClickedChooseImagePathSlot();

private:
    Ui::RegisterService *ui;
    UserInfo mUser;
    Client *mClient;
    QString mImagePath;
    //! Flags for cv::namedWindow
    enum WindowFlags {
           WINDOW_NORMAL     = 0x00000000, //!< the user can resize the window (no constraint) / also use to switch a fullscreen window to a normal size.
           WINDOW_AUTOSIZE   = 0x00000001, //!< the user cannot resize the window, the size is constrainted by the image displayed.
           WINDOW_OPENGL     = 0x00001000, //!< window with opengl support.

           WINDOW_FULLSCREEN = 1,          //!< change the window to fullscreen.
           WINDOW_FREERATIO  = 0x00000100, //!< the image expends as much as it can (no ratio constraint).
           WINDOW_KEEPRATIO  = 0x00000000, //!< the ratio of the image is respected.
           WINDOW_GUI_EXPANDED=0x00000000, //!< status bar and tool bar
           WINDOW_GUI_NORMAL = 0x00000010, //!< old fashious way
        };
};

#endif // REGISTERSERVICE_H
