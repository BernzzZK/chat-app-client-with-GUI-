//
// Created by xwz on 25-6-15.
//

#ifndef FORGETPWD_H
#define FORGETPWD_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class ForgetPwd; }
QT_END_NAMESPACE

class ForgetPwd : public QDialog {
Q_OBJECT

public:
    explicit ForgetPwd(QWidget *parent = nullptr);
    ~ForgetPwd() override;

private:
    Ui::ForgetPwd *ui;
};


#endif //FORGETPWD_H
