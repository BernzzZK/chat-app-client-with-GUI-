//
// Created by xwz on 25-6-10.
//

#include "Common.h"
#include <algorithm>
#include <QCryptographicHash>

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

// 函数：获取给定字符串的 SHA-256 哈希值
QString common::getSha256Hash(const QString& input)
{
    // 将输入字符串转换为字节数组，这里使用 UTF-8 编码
    QByteArray data = input.toUtf8();
    // 创建 QCryptographicHash 对象，并指定使用 SHA-256 算法
    QCryptographicHash hash(QCryptographicHash::Sha256);
    // 将数据添加到哈希计算器中
    hash.addData(data);
    // 返回最终的哈希值，转换为十六进制表示形式
    return hash.result().toHex();
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