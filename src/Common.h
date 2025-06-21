//
// Created by xwz on 25-6-10.
//

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <QString>
#include <vector>
#include <unordered_set>

class QWidget;

namespace common {
    const int ms = 1;
    const int second = 1000 * ms;
    const int minute = 60 * second;
    const int hour = 60 * minute;
    const int day = 24 * hour;

    enum reqType{
        nullTyp,
        registered,
        login,
        logout,
        sendMsg,
        addFriend,
        friendList,
        changeUserName,
        heartbeat
    };

    // 定义一个结构体来表示配置项
    struct ConfigEntry {
        QString comment;
        QString value;
    };

    // 定义一个结构体来表示好友项
    struct FriendEntry {
        QString account;
        QString username;
        QString createTime;
    };

    // 定义一个结构体来表示信息项
    struct MessageEntry {
        QString content;
        QString senderUsername;
        QString createTime;
    };

    std::vector<std::string> splitString(const std::string &input);
    bool containsInvalidChars(const std::string& input);
    std::string parsing(std::string);
    void removeNewline(std::string &str);
    std::string unescapeIllegalCharacters(const std::string& input);
    std::string escapeIllegalCharacters(const std::string& input);
    std::string encrypt(const std::string& input);
    std::string decrypt(const std::string &input);
    bool validatePassword(QWidget* parent, const QString &password);
    bool validateAccount(QWidget* parent, const QString &account);
    QMap<QString, QString> readSettingsFile(const QString &filePath);
    void saveConfigToFile(const QString &filePath, const QMap<QString, ConfigEntry> &configMap);

    const std::vector illegalAlphabet{' ', '@', '#', '$', '{', '}', '[', ']', ';', ':', '\'', '\"', '\\', '|', '?', '*', '%', ','};
    const std::unordered_set illegalAlphabetSet(illegalAlphabet.begin(), illegalAlphabet.end());

    reqType getType(const QString & message);
}

#endif //COMMON_H
