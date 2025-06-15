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
    QObject::connect(&client, &Client::loginSuccess, [&]() {
        const auto w = new MainWindow();
        w->show();
        loginPage.close();
    });
    return a.exec();
}
