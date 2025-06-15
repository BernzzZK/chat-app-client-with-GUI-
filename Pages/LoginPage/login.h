//
// Created by xwz on 25-6-10.
//

#ifndef LOGIN_H
#define LOGIN_H


#include <QLabel>
#include <QMouseEvent>
#include "Client.h"


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

    bool m_dragging;
    QPoint m_dragPosition;

    Client &client;

    QTimer *timer;

private slots:
    void loginBtn_clicked();
    void registerBtn_clicked();
    void forgetPwd_clicked();
    void initClientConnection();
    void onConnected();
    void onConnectTimeout();
    void changeHostBtn_clicked();
    void autoLogin();

signals:

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};


#endif //LOGIN_H
