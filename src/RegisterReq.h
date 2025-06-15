//
// Created by xwz on 25-6-12.
//

#ifndef REGISTER_H
#define REGISTER_H

#include <string>
#include "Request.h"

class RegisterReq {
public:
    RegisterReq(const std::string &acc, const std::string &pwd);
    ~RegisterReq() = default;

    std::string toString() const ;

private:
    Request req_;
    std::string acc_;
    std::string pwd_;

};



#endif //REGISTER_H
