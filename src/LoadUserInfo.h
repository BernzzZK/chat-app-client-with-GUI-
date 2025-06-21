//
// Created by xwz on 25-6-21.
//

#ifndef LOADUSERINFO_H
#define LOADUSERINFO_H

#include <QMap>
#include "Common.h"

class QSqlDatabase;

class LoadUserInfo {
public:
    explicit LoadUserInfo(const QString &account);
    ~LoadUserInfo() = default;
    void load();

    QMap<QString, QString> getUserSettings() const { return userSettings_; }
    QVector<common::FriendEntry> getFriendList() const { return friendList_; }
    QVector<common::MessageEntry> getMessageList() const { return messageList_; }

private:
    QString account_;
    QString savePath_;
    QString settingsPath_;
    QString messagePath_;
    QString friendListPath_;

    using CreateTableFunc = bool (*)(const QSqlDatabase&, const QString&);
    QMap<QString, CreateTableFunc> tableCreators_;

    QMap<QString, QString> userSettings_;
    QVector<common::FriendEntry> friendList_;
    QVector<common::MessageEntry> messageList_;


    bool isPathExist(const QString &pathname) const ;
    bool isFileExist(const QString &filename) const ;
    bool isTableExist(const QString &filename, const QString &tableName) const ;
    void createFileIfNotExists(const QString& filename, const QString& fileDescription) const;
    QVector<common::FriendEntry> loadFriendDB(const QString &filePath);
    QVector<common::MessageEntry> loadMessageDB(const QString &filePath);
};



#endif //LOADUSERINFO_H