//
// Created by xwz on 25-6-11.
//

#include "Response.h"

Response::Response(const std::string &resp)
    : _success(false), _time(QDateTime::currentDateTimeUtc()), _failReason(""), _type(common::nullTyp)
{
    toResponse(resp);
}

void Response::makeEmpty() {
    this->_type = common::nullTyp;
    this->_success = false;
    this->_time = QDateTime::currentDateTimeUtc();
    this->_failReason = "";
}


void Response::toResponse(const std::string &resp)
{
    if (resp.empty())
    {
        qDebug() << "Invalid response format";
        return;
    }
    const auto res = common::splitString(resp);
    _type = static_cast<common::reqType>(std::stoi(res[0]));
    _success = std::stoi(res[1]);
    _time = QDateTime::fromString(QString::fromStdString(res[2]), "yyyy-MM-dd HH:mm:ss");
    _failReason = res[3];
}
