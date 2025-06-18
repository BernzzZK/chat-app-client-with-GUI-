//
// Created by xwz on 25-6-17.
//

#ifndef HEARTBEATREQ_H
#define HEARTBEATREQ_H

#include "Request.h"

class HeartBeatReq {
private:
    Request req_;
    QString msg_;
public:
    HeartBeatReq(QString msg);
    ~HeartBeatReq() = default;
    std::string toString() const;
};



#endif //HEARTBEATREQ_H
