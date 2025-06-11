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

#include <QObject>
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

    void login(const QString &account, const QString &password) const;
    bool registerAccount(const QString &account, const QString &password) const;

    static Client &instance() {
        static Client client;
        return client;
    }

    QString getHost() const;
    QString getIP() const { return host_.toString(); }
    QString getPort() const { return QString::number(port_); }
    void setIP(const QString &ip) { host_ = QHostAddress(ip); }
    void setPort(const quint16 &port) { port_ = port; }
    void setServer(const QString &ip, const quint16 &port);
    void connectToServer();

signals:
    void connected();
    void connectTimedOut();
    void dataReceived(const QString& message);

    void loginSuccess();
    void loginFailed(const QString& reason);

private slots:
    void onConnected();
    void onConnectTimedOut();
    void onReadyRead();

private:
    QTcpSocket *socket_;
    QHostAddress host_;
    quint16 port_;
    QTimer *connectTimer_;

    Buffer buffer_;

    void send(const std::string &msg) const ;
    void handleResponse(const QString &message);
};

#endif //CLIENT_H
