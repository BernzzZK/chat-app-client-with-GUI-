//
// Created by xwz on 25-6-10.
//

#include "Common.h"
#include <algorithm>
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>

const std::unordered_map<int, char> encryptionAlphabet = {
    {0, 'a'}, {1, 'b'}, {2, 'c'}, {3, 'd'}, {4, 'e'}, {5, 'f'}, {6, 'g'}, {7, 'h'}, {8, 'i'}, {9, 'j'},
    {10, 'k'}, {11, 'l'}, {12, 'm'}, {13, 'n'}, {14, 'o'}, {15, 'p'}, {16, 'q'}, {17, 'r'}, {18, 's'}, {19, 't'},
    {20, 'u'}, {21, 'v'}, {22, 'w'}, {23, 'x'}, {24, 'y'}, {25, 'z'}, {26, 'A'}, {27, 'B'}, {28, 'C'}, {29, 'D'},
    {30, 'E'}, {31, 'F'}, {32, 'G'}, {33, 'H'}, {34, 'I'}, {35, 'J'}, {36, 'K'}, {37, 'L'}, {38, 'M'}, {39, 'N'},
    {40, 'O'}, {41, 'P'}, {42, 'Q'}, {43, 'R'}, {44, 'S'}, {45, 'T'}, {46, 'U'}, {47, 'V'}, {48, 'W'}, {49, 'X'},
    {50, 'Y'}, {51, 'Z'}, {52, '0'}, {53, '1'}, {54, '2'}, {55, '3'}, {56, '4'}, {57, '5'}, {58, '6'}, {59, '7'},
    {60, '8'}, {61, '9'}, {62, '!'}, {63, '^'}, {64, '&'}, {65, '('}, {66, ')'}, {67, '-'}, {68, '_'}, {69, '+'},
    {70, '='}, {71, '`'}, {72, '~'}, {73, ','}, {74, '.'}, {75, '<'}, {76, '>'}
};

std::vector<std::string> common::splitString(const std::string &input)
{
    std::vector<std::string> elements;
    if (input.empty()) {
        return elements;
    }
    char delimiter = input[0];
    std::string token;
    bool escaped = false;
    for (size_t i = 1; i < input.size(); ++i) {
        char c = input[i];
        if (escaped) {
            token += c;
            escaped = false;
        } else if (c == '|') {
            token += '|';
            escaped = true;
        } else if (c == delimiter) {
            elements.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    elements.push_back(token);
    if (!input.empty() && input.back() == delimiter) {
        elements.emplace_back("");
    }
    return elements;
}


bool common::containsInvalidChars(const std::string &input) {
    for (char c : input) {
        if (find(illegalAlphabet.begin(), illegalAlphabet.end(), c) != illegalAlphabet.end()) {
            return true;
        }
    }
    return false;
}

std::string common::parsing(std::string msg) {
    auto res = common::splitString(msg);
    return res[0];
}

void common::removeNewline(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

// 反向映射：字符 -> 下标
const std::unordered_map<char, int> reverseAlphabet = []() {
    std::unordered_map<char, int> map;
    for (const auto& pair : encryptionAlphabet) {
        map[pair.second] = pair.first;
    }
    return map;
}();

std::string common::encrypt(const std::string &input) {
    if (input.empty()) return "";
    // 计算字符下标总和
    int sumIndex = 0;
    for (char c : input) {
        auto it = reverseAlphabet.find(c);
        if (it == reverseAlphabet.end()) {
            throw std::invalid_argument("Character not in alphabet: " + std::string(1, c));
        }
        sumIndex += it->second;
    }
    // 计算偏移量
    int length = static_cast<int>(input.size());
    int shift = sumIndex % length;
    if (shift == 0) {
        shift = 1;  // 偏移为0时强制设为1
    }
    // 加密每个字符
    std::string encrypted;
    for (char c : input) {
        int originalIndex = reverseAlphabet.at(c);
        int newIndex = (originalIndex + shift) % 77;
        encrypted += encryptionAlphabet.at(newIndex);
    }
    return encrypted;
}

std::string common::decrypt(const std::string &input) {
    if (input.empty()) return "";

    int length = static_cast<int>(input.size());

    // 构建反向映射：字符 -> 下标
    const std::unordered_map<char, int> reverseAlphabet = []() {
        std::unordered_map<char, int> map;
        for (const auto& pair : encryptionAlphabet) {
            map[pair.second] = pair.first;
        }
        return map;
    }();

    // 只尝试 [1, length] 范围内的偏移量
    for (int shift_candidate = 1; shift_candidate <= length; ++shift_candidate) {
        std::string decrypted_candidate;

        // 解密每个字符
        for (char c : input) {
            auto it = reverseAlphabet.find(c);
            if (it == reverseAlphabet.end()) {
                throw std::invalid_argument("Character not in alphabet: " + std::string(1, c));
            }

            int encryptedIndex = it->second;
            int originalIndex = (encryptedIndex - shift_candidate + 77) % 77;
            decrypted_candidate += encryptionAlphabet.at(originalIndex);
        }

        // 验证候选明文是否合法：重新计算偏移量并比较
        int sumIndex_recomputed = 0;
        for (char c : decrypted_candidate) {
            auto it = reverseAlphabet.find(c);
            if (it == reverseAlphabet.end()) {
                throw std::invalid_argument("Character not in alphabet: " + std::string(1, c));
            }
            sumIndex_recomputed += it->second;
        }

        int shift_recomputed = sumIndex_recomputed % length;
        if (shift_recomputed == 0) {
            shift_recomputed = 1;
        }

        if (shift_recomputed == shift_candidate) {
            return decrypted_candidate;
        }
    }

    throw std::runtime_error("Unable to decrypt the message - no valid plaintext found");
}


std::string common::unescapeIllegalCharacters(const std::string& input) {
    std::string result;
    size_t i = 0;
    while (i < input.length()) {
        // 检查是否是转义字符 |
        if (input[i] == '|' && i + 1 < input.length()) {
            char nextChar = input[i + 1];
            // 如果下一个字符是非法字符，则进行还原
            if (illegalAlphabetSet.find(nextChar) != illegalAlphabetSet.end()) {
                result += nextChar;
                i += 2;  // 跳过这两个字符
                continue;
            }
        }
        // 否则直接添加当前字符
        result += input[i];
        i++;
    }
    return result;
}

std::string common::escapeIllegalCharacters(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (common::illegalAlphabetSet.find(c) != common::illegalAlphabetSet.end()) {
            result += '|';
        }
        result += c;
    }
    return result;
}

QString getIllegalChars() {
    QString illegalChars;
    for (const char c : common::illegalAlphabet) {
        illegalChars += QString(c) + " ";
    }
    return illegalChars;
}

bool common::validateAccount(QWidget* parent, const QString &account) {
    if (account.isEmpty()) {
        QMessageBox::warning(parent, "警告", "账号不能为空");
        return false;
    }
    if (account.size() < 4 || account.size() > 16) {
        QMessageBox::warning(parent, "警告", "账号长度必须在4到16之间");
        return false;
    }
    if (common::containsInvalidChars(account.toStdString())) {
        QString illegalChars = getIllegalChars();
        QMessageBox::warning(parent, "警告",
                             QString("用户名不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                             .arg(illegalChars));
        return false;
    }
    return true;
}

bool common::validatePassword(QWidget* parent, const QString &password) {
    if (password.isEmpty()) {
        QMessageBox::warning(parent, "警告", "密码不能为空");
        return false;
    }
    if (password.size() < 4 || password.size() > 16) {
        QMessageBox::warning(parent, "警告", "密码长度必须在4到16之间");
        return false;
    }
    if (common::containsInvalidChars(password.toStdString())) {
        QString illegalChars = getIllegalChars();
        QMessageBox::warning(parent, "警告",
                             QString("密码不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                             .arg(illegalChars));
        return false;
    }
    return true;
}

QMap<QString, QString> common::readSettingsFile(const QString &filePath) {
    QMap<QString, QString> settings;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件：" << filePath;
        return settings;
    }
    QTextStream in(&file);
    QString comment;  // 存储当前注释（可选）
    int lineNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNum++;
        if (line.isEmpty()) {
            continue; // 跳过空行
        }
        if (line.startsWith("#")) {
            // 当前行为注释
            comment = line.mid(1).trimmed(); // 去掉 # 并去前后空格
        } else {
            // 当前行为 key=value
            int sepIndex = line.indexOf('=');
            if (sepIndex == -1) {
                qWarning() << "无效格式（缺少 '='）第" << lineNum << "行：" << line;
                continue;
            }
            QString key = line.left(sepIndex).trimmed();
            QString value = line.mid(sepIndex + 1).trimmed();
            if (key.isEmpty()) {
                qWarning() << "无效格式（键为空）第" << lineNum << "行：" << line;
                continue;
            }
            settings[key] = value;
            // 如果需要可以输出带注释信息
            qDebug() << key << "=" << value << "  #" << comment;
            comment.clear(); // 清除当前注释
        }
    }
    file.close();
    return settings;
}

void common::saveConfigToFile(const QString &filePath, const QMap<QString, ConfigEntry> &configMap) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入：" << filePath;
        return;
    }

    QTextStream out(&file);

    for (auto it = configMap.begin(); it != configMap.end(); ++it) {
        const QString &key = it.key();
        const ConfigEntry &entry = it.value();

        // 写入注释和键值对
        out << "# " << key << "\n";
        out << key << " = " << entry.value << "\n\n";
    }

    file.close();
}

common::reqType common::getType(const QString &message) {
    auto msg = message.toStdString();
    return static_cast<reqType>(msg[msg.size()]);
}