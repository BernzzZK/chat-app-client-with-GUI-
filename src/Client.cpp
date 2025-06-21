//
// Created by xwz on 25-6-10.
//

#include "Client.h"
#include "Common.h"
#include <QDebug>
#include "HeartBeatReq.h"
#include "LoginReq.h"
#include "Response.h"
#include "RegisterReq.h"

Client::Client()
    : socket_(new QTcpSocket(this)),
      host_("127.0.0.1"),
      port_(8888),
      connectTimer_(new QTimer(this)),
      heartbeatTimer_(new QTimer(this)),
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

    connectTimer_->setInterval(3 * common::second);
    connectTimer_->setSingleShot(true);

    heartbeatTimer_->setInterval(30 * common::second); // 30秒发送一次心跳

    heartbeatResponseTimer_ = new QTimer(this);
    heartbeatResponseTimer_->setInterval(5 * common::second); // 等待心跳响应最多5秒
    heartbeatResponseTimer_->setSingleShot(true);

    connect(heartbeatResponseTimer_, &QTimer::timeout, this, &Client::onHeartbeatResponseTimeout);

    receivedHeartbeatResponse_ = true; // 初始设为 true，避免第一次误判

    reconnectTimer_ = new QTimer(this);
    reconnectTimer_->setInterval(3 * common::second); // 每3秒尝试连接一次
    reconnectTimer_->setSingleShot(false);

    connect(reconnectTimer_, &QTimer::timeout, this, &Client::connectToServer);

    connect(socket_, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(heartbeatTimer_, &QTimer::timeout, this, &Client::sendHeartbeat);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(connectTimer_, &QTimer::timeout, this, &Client::onConnectTimedOut);
    connect(this, &Client::loginSuccess, this, [this]() {
        logined_ = true;
        // 登录成功后，停止重连定时器
        reconnectTimer_->stop();
        // 读取用户信息、设置等
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
    if (socket_->state() == QTcpSocket::ConnectedState) {
        return; // 已连接，无需重复连接
    }

    qDebug() << "[连接] 尝试连接服务器..." << host_.toString() << ":" << port_;

    socket_->abort(); // 防止残留连接影响
    socket_->connectToHost(host_, port_);

    connectTimer_->start(); // 启动连接超时检测
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
    qDebug() << "[连接] 成功连接到服务器！";
    connectTimer_->stop();
    reconnectTimer_->stop(); // 停止重连定时器
    heartbeatTimer_->start(); // 开始发送心跳包
    emit connected();
}

void Client::onDisconnected() {
    qDebug() << "与服务器断开连接，准备重连...";
    heartbeatTimer_->stop();       // 停止心跳
    connectTimer_->stop();         // 停止连接超时计时器
    reconnectTimer_->start();      // 启动循环重连定时器
}

void Client::sendHeartbeat() {
    if (socket_->state() == QTcpSocket::ConnectedState) {
        HeartBeatReq heart("heartbeat");
        send(heart.toString());

        qDebug() << "发送心跳请求...";
        receivedHeartbeatResponse_ = false;
        heartbeatResponseTimer_->start(); // 启动等待响应定时器
    }
}

void Client::onConnectTimedOut() {
    emit connectTimedOut();
}

void Client::onReadyRead() {
    while (socket_->bytesAvailable() > 0) {
        QByteArray data = socket_->read(1024); // 每次最多读 1024 字节
        buffer_.append(data.data(), data.size()); // 使用 Buffer 的 append 方法
    }

    // 用指针操作来解析 buffer_
    const char* begin = buffer_.peek(); // 获取当前可读起始位置
    int readable = buffer_.readableBytes();

    int pos = 0;
    while (pos < readable) {
        // 查找 \r\n 的位置
        const char* crlf = static_cast<const char*>(memchr(begin + pos, '\r', readable - pos));
        if (crlf == nullptr || crlf + 1 >= begin + readable || *(crlf + 1) != '\n') {
            // 找不到完整的 \r\n，跳出循环等待后续数据
            break;
        }

        // 计算这条消息的长度（不包含 \r\n）
        int msgLen = crlf - begin;

        // 提取消息内容（不含 \r\n）
        QString message = QString::fromUtf8(begin, msgLen);

        if (!message.isEmpty()) {
            qDebug() << "收到服务器消息：" << message;

            if (message.startsWith("@") || message.startsWith("#")) {
                handleResponse(message);
            } else {
                emit dataReceived(message);
            }
        }

        // 更新 pos 到 \r\n 后面
        pos += msgLen + 2; // 跳过 \r\n

        // 移动 readerIndex_ 到已处理的位置
        buffer_.retrieve(msgLen + 2);
        begin = buffer_.peek(); // 更新指针
        readable = buffer_.readableBytes();
    }

    // 如果还有剩余未处理的数据，保持在 buffer_ 中，等待下次 onReadyRead()
}

void Client::send(const std::string &msg) const {
    socket_->write(msg.c_str(), msg.size());
    socket_->flush();
}

void Client::handleResponse(const QString &message) {
    common::reqType type = common::nullTyp;
    Response resp;
    if (message.startsWith("@")) {
        resp = Response(message.toStdString());
        type = resp.getType();
    } else if (message.startsWith("#")) {
        type = common::getType(message);
    }
    qDebug() << "type: " << type << " msg: " << message;
    switch (type) {
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
        case common::heartbeat: {
            receivedHeartbeatResponse_ = true; // 标记已收到心跳响应
            break;
        }
        case common::addFriend : {
            qDebug() << "接收到添加好友请求";
            break;
        }
        case common::friendList : {
            qDebug() << "接收到好友列表";
            break;
        }
        case common::sendMsg : {
            qDebug() << "接收到消息";
            break;
        }
        default:
            qDebug() << "未知响应类型";
            break;
    }
}

void Client::initSettings(const QString &fileName) {
    QMap<QString, QString> settings = common::readSettingsFile(fileName);
    if (!settings.isEmpty()) {
        for (const auto &key : settings.keys()) {
            if (setters_.contains(key)) {
                setters_[key](settings[key]);
            }
        }
    }
}

void Client::onHeartbeatResponseTimeout() const {
    if (!receivedHeartbeatResponse_) {
        qDebug() << "超时未收到心跳响应，可能已断开连接，尝试重新连接...";
        socket_->disconnectFromHost(); // 主动断开，触发重连机制
    } else {
        qDebug() << "成功收到心跳响应，连接正常";
    }
}
