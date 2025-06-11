//
// Created by xwz on 25-6-11.
//

#include "LoginReq.h"
#include <sstream>

LoginReq::LoginReq(std::string acc, std::string pwd)
        : acc_(acc)
        , pwd_(pwd)
        , req_(Request(common::login))
{

}

std::string LoginReq::toString() const  {
        std::stringstream ss;
        ss << req_.toString() << "@#" << acc_ << "#" << pwd_;
        return ss.str();
}




