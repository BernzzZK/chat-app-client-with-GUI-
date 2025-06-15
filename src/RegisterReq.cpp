//
// Created by xwz on 25-6-12.
//

#include "RegisterReq.h"
#include <sstream>

RegisterReq::RegisterReq(const std::string &acc, const std::string &pwd)
    : req_(common::registered)
    , acc_(acc)
    , pwd_(pwd)
{
}

std::string RegisterReq::toString() const {
    std::stringstream ss;
    ss << req_.toString() << "@#" << acc_ << "#" << pwd_;
    return ss.str();
}
