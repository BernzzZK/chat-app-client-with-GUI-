//
// Created by xwz on 25-6-11.
//

#include "Request.h"
#include <sstream>

Request::Request(const common::reqType reqType, const QDateTime &reqTime)
    : reqType_(reqType)
    , reqTime_(reqTime)
{
}

std::string Request::toString() const {
    std::stringstream ss;
    ss << "@" << reqType_ << "@" << reqTime_.toString("yyyy-MM-dd HH:mm:ss").toStdString();
    return ss.str();
}

void Request::toReq(const std::string& info) {
    const auto res = common::splitString(info);
    if (res.size() == 2) {
        reqType_ = static_cast<common::reqType>(stoi(res[0]));
        reqTime_ = QDateTime::fromString(QString::fromStdString(res[1]), "yyyy-MM-dd HH:mm:ss");
    } else {
        throw std::runtime_error("Invalid request format");
    }
}

