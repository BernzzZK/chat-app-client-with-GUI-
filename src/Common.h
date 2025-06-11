//
// Created by xwz on 25-6-10.
//

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <QString>
#include <vector>
#include <unordered_set>

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
    std::vector<std::string> splitString(const std::string &input);
    bool containsInvalidChars(const std::string& input);
    std::string parsing(std::string);
    void removeNewline(std::string &str);
    std::string unescapeIllegalCharacters(const std::string& input);
    std::string escapeIllegalCharacters(const std::string& input);
    QString getSha256Hash(const QString& input);

    const std::vector illegalAlphabet{' ', '@', '#', '$', '{', '}', '[', ']', ';', ':', '\'', '\"', '\\', '|', '?', '*', '%', ','};
    const std::unordered_set illegalAlphabetSet(illegalAlphabet.begin(), illegalAlphabet.end());
}

#endif //COMMON_H
