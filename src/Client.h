//
// Created by xwz on 25-6-10.
//

/**
 * 客户端后端
 * 用于处理客户端的请求
 * 包括登录、注册、聊天等
 * 客户端的请求会通过网络发送到服务器
 * 服务器会根据请求的类型进行处理
 * 处理完成后，服务器会将结果返回给客户端
 * 客户端会根据返回的结果进行处理
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QTimer>
#include <QString>
#include "Buffer.h"

class Response;

class Client : public QObject {
    Q_OBJECT
public:
    Client();
    ~Client() override;

    void login(const QString &account, const std::string &password) const;
    void registerAccount(const QString &account, const std::string &password) const;

    static Client &instance() {
        static Client client;
        return client;
    }

    void setAutoLogin(const bool autoLogin) { autoLogin_ = autoLogin; }
    void setRememberMe(const bool setRememberMe) { rememberMe_ = setRememberMe;}
    bool getAutoLogin() const {return autoLogin_;}
    bool getRememberMe() const {return rememberMe_;}
    bool isLogin() const {return logined_;}

    void setAccount(const QString &account) { account_ = account; }
    void setPassword(const QString &password) { password_ = password; }
    QString getAccount() const { return account_; }
    QString getPassword() const { return password_; }

    void initSettings(QString fileName);

    QString getHost() const;
    QString getIP() const { return host_.toString(); }
    QString getPort() const { return QString::number(port_); }
    void setIP(const QString &ip) { host_ = QHostAddress(ip); }
    void setPort(const quint16 &port) { port_ = port; }
    void setServer(const QString &ip, const quint16 &port);
    void connectToServer();
    bool isConnected() const { return socket_->state() == QAbstractSocket::ConnectedState; };

signals:
    void connected();
    void connectTimedOut();
    void dataReceived(const QString& message);

    void loginSuccess();
    void loginFailed(const QString& reason);

    void registerSuccess();
    void registerFailed(const QString& reason);

private slots:
    void onConnected();
    void onConnectTimedOut();
    void onReadyRead();

private:
    QTcpSocket *socket_;
    QHostAddress host_;
    quint16 port_;
    QTimer *connectTimer_;

    // default false
    bool autoLogin_;
    bool rememberMe_;

    QString account_;
    QString password_;

    Buffer buffer_;

    bool logined_;

    QMap<QString, std::function<void(const QString&)>> setters_; // 字符串映射至函数表

    void send(const std::string &msg) const ;
    void handleResponse(const QString &message);
    void saveInitSettings();
};

#endif //CLIENT_H
