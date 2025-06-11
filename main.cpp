#include "mainwindow.h"
#include "Pages/LoginPage/login.h"
#include "src/Client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login loginPage;
    QObject::connect(&loginPage, &Login::loginSuccess, [&]() {
        const auto w = new MainWindow();
        w->show();
        loginPage.close();
    });
    loginPage.show();
    return a.exec();
}
