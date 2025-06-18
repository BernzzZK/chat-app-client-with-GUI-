//
// Created by xwz on 25-6-11.
//

#ifndef RESPONSE_H
#define RESPONSE_H

#include <QDateTime>
#include "Common.h"

class Response {
public:
    Response(const std::string &resp);
    Response() = default;
    ~Response() = default;

    void toResponse(const std::string &);

    void makeEmpty();

    bool isSuccess() const { return _success; }
    std::string getFailReason() const { return _failReason; }
    common::reqType getType() const { return _type; }
private:
    common::reqType _type;
    bool _success;
    QDateTime _time;
    std::string _failReason;
};



#endif //RESPONSE_H
