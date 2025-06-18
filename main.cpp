#include "mainwindow.h"
#include "Pages/LoginPage/login.h"
#include "src/Client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client &client = Client::instance();
    client.initSettings("settings.ini");
    Login loginPage;
    loginPage.show();
    return a.exec();
}
