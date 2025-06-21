//
// Created by xwz on 25-6-19.
//

#include "SendMsgReq.h"

SendMsgReq::SendMsgReq(const QString &msg, const QString &receiver, const QString &sender)
    : msg_(msg)
    , receiver_(receiver)
    , sender_(sender)
    , reqHead_(common::sendMsg)
{
}

std::string SendMsgReq::toString() const {
    return reqHead_.toString() + "@#" + msg_.toStdString() + "#" + receiver_.toStdString() + "#" + sender_.toStdString() ;
}
