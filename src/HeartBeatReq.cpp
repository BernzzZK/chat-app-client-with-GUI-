//
// Created by xwz on 25-6-17.
//

#include "HeartBeatReq.h"

HeartBeatReq::HeartBeatReq(QString msg)
    : req_(common::heartbeat)
    , msg_(msg)
{
}

std::string HeartBeatReq::toString() const {
    return req_.toString() + "@" + msg_.toStdString();
}
