//
// Created by xwz on 25-6-10.
//

#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget {
Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

private:
    Ui::Login *ui;

private slots:
    void loginBtn_clicked();
    void registerBtn_clicked();
    void initClientConnection();
    void onConnected();
    void onConnectTimeout();
    void changeHostBtn_clicked();

signals:
    void loginSuccess();
    void registerSuccess();
};


#endif //LOGIN_H
