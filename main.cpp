#include "manager/managerservice.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ManagerService *manager = new ManagerService();

    return a.exec();
}
