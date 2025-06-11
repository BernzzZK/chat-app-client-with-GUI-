//
// Created by xwz on 25-6-10.
//

#include "Client.h"
#include "Common.h"
#include <QDebug>
#include "LoginReq.h"
#include "Response.h"

Client::Client()
    : socket_(new QTcpSocket(this)),
      host_("127.0.0.1"),
      port_(8888),
      connectTimer_(new QTimer(this))
{
    connect(socket_, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connectTimer_->setSingleShot(true);
    connect(connectTimer_, &QTimer::timeout, this, &Client::onConnected);
}

Client::~Client() {
    delete socket_;
}

QString Client::getHost() const {
    if (socket_->state() != QAbstractSocket::ConnectedState) {
        return "未连接";
    }
    return "连接成功：" + QString("%1:%2").arg(socket_->peerAddress().toString()).arg(socket_->peerPort());
}

void Client::connectToServer() {
    socket_->connectToHost(host_, port_);
    connectTimer_->start(3000);
}

void Client::setServer(const QString &ip, const quint16 &port) {
    host_ = QHostAddress(ip);
    port_ = port;
}

void Client::login(const QString &account, const QString &password) const {
    LoginReq req(account.toStdString(), password.toStdString());
    if (socket_->state()!= QAbstractSocket::ConnectedState) {
        return;
    }
    send(req.toString());
}

bool Client::registerAccount(const QString &account, const QString &password) const {
    return false;
}

void Client::onConnected() {
    connectTimer_->stop(); // 停止计时器
    emit connected();
}

void Client::onConnectTimedOut() {
    emit connected();
}

void Client::onReadyRead() {
    while (socket_->bytesAvailable() > 0) {
        QByteArray data = socket_->read(1024); // 每次最多读 1024 字节
        buffer_.append(data);                  // 添加进缓冲区
    }
    // 提取所有内容并清空缓冲区
    QString received = buffer_.retrieveAllAsString();
    if (!received.isEmpty()) {
        qDebug() << "收到服务器消息：" << received;
        if (received.startsWith("@")) {
            handleResponse(received);
        } else {
            emit dataReceived(received);
        }
    }
}

void Client::send(const std::string &msg) const {
    socket_->write(msg.c_str(), msg.size());
    socket_->flush();
}

void Client::handleResponse(const QString &message) {
    Response resp(message.toStdString());
    switch (resp.getType()) {
        case common::login: {
            if (resp.isSuccess()) {
                emit loginSuccess();
            } else {
                emit loginFailed(QString::fromStdString(resp.getFailReason()));
            }
            break;
        }
        default:
            qDebug() << "未知响应类型";
            break;
    }
}

