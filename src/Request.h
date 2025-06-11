//
// Created by xwz on 25-6-11.
//

#ifndef REQUEST_H
#define REQUEST_H

#include <QDateTime>
#include "Common.h"

class Request {
public:
    explicit Request(common::reqType reqType, const QDateTime &reqTime = QDateTime::currentDateTimeUtc());
    ~Request() = default;

    std::string toString() const ;
    void toReq(const std::string &info);
private:
    common::reqType reqType_;
    QDateTime reqTime_;
};



#endif //REQUEST_H
