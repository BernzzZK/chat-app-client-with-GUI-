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
    enum reqType{
        nullTyp,
        registered,
        login,
        logout,
        sendMsg,
        addFriend,
        friendList
    };

    // 定义一个结构体来表示配置项
    struct ConfigEntry {
        QString comment;
        QString value;
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
}

#endif //COMMON_H
