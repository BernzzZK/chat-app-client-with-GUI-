//
// Created by xwz on 25-6-10.
//

#include "Client.h"
#include "Common.h"
#include <QDebug>
#include "LoginReq.h"
#include "Response.h"
#include "RegisterReq.h"

Client::Client()
    : socket_(new QTcpSocket(this)),
      host_("127.0.0.1"),
      port_(8888),
      connectTimer_(new QTimer(this)),
      autoLogin_(false),
      rememberMe_(false)
{
    // 初始化 setter 映射表
    setters_["host"] = [this](const QString &val) {
        host_ = QHostAddress(val);
    };

    setters_["port"] = [this](const QString &val) {
        bool ok;
        quint16 port = val.toUInt(&ok);
        if (ok) {
            port_ = port;
            qDebug() << "设置 port 为：" << port_;
        } else {
            qWarning() << "端口号无效：" << val;
        }
    };

    setters_["autoLogin"] = [this](const QString &val) {
        autoLogin_ = (val.toLower() == "true" || val == "1");
        qDebug() << "autoLogin" << autoLogin_;
    };

    setters_["rememberMe"] = [this](const QString &val) {
        rememberMe_ = (val.toLower() == "true" || val == "1");
        qDebug() << "rememberMe" << rememberMe_;
    };

    setters_["account"] = [this](const QString &val) {
        account_ = val;
    };

    setters_["password"] = [this](const QString &val) {
        password_ = val;
    };

    connect(socket_, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connectTimer_->setSingleShot(true);
    connect(connectTimer_, &QTimer::timeout, this, &Client::onConnectTimedOut);
    connect(this, &Client::loginSuccess, this, [this]() {
        logined_ = true;
    });
}

Client::~Client() {
    delete socket_;
    saveInitSettings();
}

void Client::saveInitSettings() {
    common::saveConfigToFile("settings.ini", {
        {"host", {"host", host_.toString()}},
        {"port", {"port", QString::number(port_)}},
        {"autoLogin", {"autoLogin", autoLogin_ ? "true" : "false"}},
        {"rememberMe", {"rememberMe", rememberMe_ ? "true" : "false"}},
        {"account", {"account", account_}},
        {"password", {"password", password_}}
    });
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

void Client::login(const QString &account, const std::string &password) const {
    LoginReq req(account.toStdString(), password);
    if (socket_->state()!= QAbstractSocket::ConnectedState) {
        return;
    }
    send(req.toString());
}

void Client::registerAccount(const QString &account, const std::string &password) const {
    RegisterReq req(account.toStdString(), password);
    qDebug() << req.toString();
    if (socket_->state()!= QAbstractSocket::ConnectedState) {
        return;
    }
    send(req.toString());
}

void Client::onConnected() {
    connectTimer_->stop(); // 停止计时器
    emit connected();
}

void Client::onConnectTimedOut() {
    emit connectTimedOut();
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
        case common::registered: {
            if (resp.isSuccess()) {
                emit registerSuccess();
            } else {
                emit registerFailed(QString::fromStdString(resp.getFailReason()));
            }
            break;
        }
        default:
            qDebug() << "未知响应类型";
            break;
    }
}

void Client::initSettings(QString fileName) {
    QMap<QString, QString> settings;
    settings = common::readSettingsFile(fileName);
    if (!settings.isEmpty()) {
        for (const auto &key : settings.keys()) {
            if (setters_.contains(key)) {
                setters_[key](settings[key]);
            }
        }
    }
}


