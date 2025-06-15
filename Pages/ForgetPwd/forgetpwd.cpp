//
// Created by xwz on 25-6-15.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ForgetPwd.h" resolved

#include "forgetpwd.h"
#include "ui_ForgetPwd.h"


ForgetPwd::ForgetPwd(QWidget *parent) :
    QDialog(parent), ui(new Ui::ForgetPwd) {
    ui->setupUi(this);
}

ForgetPwd::~ForgetPwd() {
    delete ui;
}
