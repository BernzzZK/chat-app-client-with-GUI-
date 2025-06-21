//
// Created by xwz on 25-6-19.
//

#ifndef SENDMSGREQ_H
#define SENDMSGREQ_H

#include "Request.h"

class SendMsgReq {
public:
    SendMsgReq() = delete;
    SendMsgReq(const QString& msg, const QString& receiver, const QString& sender);
    ~SendMsgReq() = default;

    std::string toString() const;

private:
    Request reqHead_;
    QString msg_;
    QString receiver_;
    QString sender_;
};



#endif //SENDMSGREQ_H
