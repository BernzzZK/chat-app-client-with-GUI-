//
// Created by xwz on 25-6-21.
//

#include "LoadUserInfo.h"
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

LoadUserInfo::LoadUserInfo(const QString &account)
    : account_(account)
{
    this->savePath_ = QString("data/%1").arg(account_);
    this->settingsPath_ = QString("data/%1/settings.ini").arg(account_);
    this->friendListPath_ = QString("data/%1/friendList.sql").arg(account_);
    this->messagePath_ = QString("data/%1/message.sql").arg(account_);

    tableCreators_["message"] = [](const QSqlDatabase& db, const QString& tableName) -> bool {
        QSqlQuery query(db);
        return query.exec(
            "CREATE TABLE IF NOT EXISTS " + tableName + " ("
            "ID TEXT PRIMARY KEY, "
            "content TEXT NOT NULL, "
            "createTime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, "
            "senderUsername TEXT NOT NULL, "
            "senderAccount TEXT NOT NULL"
            ")"
        );
    };

    tableCreators_["friendList"] = [](const QSqlDatabase& db, const QString& tableName) -> bool {
        QSqlQuery query(db);
        return query.exec(
            "CREATE TABLE IF NOT EXISTS " + tableName + " ("
            "ID TEXT PRIMARY KEY, "
            "friendAccount TEXT, "
            "friendUsername TEXT NOT NULL, "
            "createTime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP"
            ")"
        );
    };
}

void LoadUserInfo::load() {
    if (!isPathExist(savePath_)) {
        qDebug() << "路径不存在";
        if (QDir().mkdir(savePath_))
            qDebug() << "创建用户路径成功";
        else
            qDebug() << "创建用户路径失败";
    }
    createFileIfNotExists(settingsPath_, "设置");
    createFileIfNotExists(friendListPath_, "好友列表");
    createFileIfNotExists(messagePath_, "消息");

    if (!isTableExist(friendListPath_, "friendList")) {
        qDebug() << "创建好友列表表失败";
    }
    if (!isTableExist(messagePath_, "message")) {
        qDebug() << "创建消息表失败";
    }

    userSettings_ = common::readSettingsFile(settingsPath_);
    friendList_ = loadFriendDB(friendListPath_);
    messageList_ = loadMessageDB(messagePath_);
}

bool LoadUserInfo::isPathExist(const QString &pathname) const {
    if (!QDir(pathname).exists()) {
        return false;
    }
    return true;
}

bool LoadUserInfo::isFileExist(const QString &filename) const {
    if (!QFile(filename).exists()) {
        return false;
    }
    return true;
}

bool LoadUserInfo::isTableExist(const QString &filename, const QString &tableName) const {
    if (!isPathExist(filename)) {
        qDebug() << "数据库文件不存在：" << filename;
        return false;
    }

    // 创建或连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "LoadUserInfo");
    db.setDatabaseName(filename);

    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
        return false;
    }

    // 查询所有表名
    QStringList tables = db.tables();

    if (!tables.contains(tableName, Qt::CaseInsensitive)) {
        qDebug() << "表" << tableName << "不存在，正在创建...";
        const bool success = tableCreators_[filename](db, tableName);
        return success;
    }

    db.close();
    return true;
}


void LoadUserInfo::createFileIfNotExists(const QString& filename, const QString& fileDescription) const {
    if (!isFileExist(filename)) {
        qDebug() << fileDescription << "文件不存在";
        if (QFile(filename).open(QIODevice::WriteOnly)) {
            qDebug() << "创建" << fileDescription << "文件成功";
        } else {
            qDebug() << "创建" << fileDescription << "文件失败";
        }
    }
}

QVector<common::FriendEntry> LoadUserInfo::loadFriendDB(const QString &filePath) {
    QVector<common::FriendEntry> dataList;

    // 创建唯一的数据库连接名
    QString connectionName = "readConnection_" + QString::number(qHash(filePath));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(filePath);

    if (!db.open()) {
        qWarning() << "无法打开数据库：" << db.lastError().text();
        QSqlDatabase::removeDatabase(connectionName); // 清理连接
        return dataList;
    }

    QString queryString = "SELECT account, username, createTime FROM friendList";
    QSqlQuery query(db);

    if (!query.exec(queryString)) {
        qWarning() << "查询失败：" << query.lastError().text();
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return dataList;
    }

    while (query.next()) {
        common::FriendEntry entry;
        entry.account     = query.value(0).toString(); // account
        entry.username    = query.value(1).toString(); // username
        entry.createTime  = query.value(2).toString(); // createTime
        dataList.append(entry);
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName); // 清理连接

    return dataList;
}

QVector<common::MessageEntry> LoadUserInfo::loadMessageDB(const QString &filePath) {
    QVector<common::MessageEntry> dataList;

    // 创建唯一的数据库连接名
    QString connectionName = "readMessageConnection_" + QString::number(qHash(filePath));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(filePath);

    if (!db.open()) {
        qWarning() << "无法打开数据库：" << db.lastError().text();
        QSqlDatabase::removeDatabase(connectionName); // 清理连接
        return dataList;
    }

    QString queryString = "SELECT content, senderUsername, createTime FROM message";
    QSqlQuery query(db);

    if (!query.exec(queryString)) {
        qWarning() << "查询失败：" << query.lastError().text();
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return dataList;
    }

    while (query.next()) {
        common::MessageEntry entry;
        entry.content        = query.value(0).toString(); // content
        entry.senderUsername = query.value(1).toString(); // senderUsername
        entry.createTime     = query.value(2).toString(); // createTime
        dataList.append(entry);
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName); // 清理连接

    return dataList;
}