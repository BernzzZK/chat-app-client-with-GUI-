//
// Created by xwz on 25-6-11.
//

#ifndef LOGINREQ_H
#define LOGINREQ_H

#include "Request.h"

class QString;

class LoginReq {
public:
    LoginReq(std::string acc, std::string pwd);
    ~LoginReq() = default;

    std::string toString() const;
private:
    std::string acc_;
    std::string pwd_;
    Request req_;
};



#endif //LOGINREQ_H
