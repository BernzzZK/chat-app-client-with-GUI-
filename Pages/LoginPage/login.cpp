//
// Created by xwz on 25-6-10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Login.h" resolved

#include "login.h"
#include "ui_Login.h"
#include "../../src/Client.h"
#include "../../src/Common.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFormLayout>


Login::Login(QWidget *parent) :
    QWidget(parent), ui(new Ui::Login) {

    ui->setupUi(this);

    QFont btnFont("Microsoft YaHei", 12, QFont::Bold);
    ui->loginBtn->setFont(btnFont);
    ui->registerBtn->setFont(btnFont);

    ui->changeHostBtn->setText("切换服务器");

    initClientConnection();
    connect(ui->loginBtn, &QPushButton::clicked, this, &Login::loginBtn_clicked);
    connect(ui->changeHostBtn, &QPushButton::clicked, this, &Login::changeHostBtn_clicked);

}

Login::~Login() {
    delete ui;
}

void Login::initClientConnection() {
    ui->labHost->setText("连接中...");
    Client &client = Client::instance();
    connect(&client, &Client::connected, this, &Login::onConnected);
    connect(&client, &Client::connectTimedOut, this, &Login::onConnectTimeout);
    connect(ui->changeHostBtn, &QPushButton::clicked, this, [this]() {
        ui->labHost->setText("切换中...");
    });
    client.connectToServer();
}

void Login::onConnected() {
    ui->labHost->setText(Client::instance().getHost());
}

void Login::onConnectTimeout() {
    ui->labHost->setText("连接超时");
}

void Login::loginBtn_clicked() {
    QString account = ui->accInput->text();
    QString password = ui->pwdInput->text();

    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "账号或密码不能为空");
        return;
    }

    if (account.size() < 4 || account.size() > 16) {
        QMessageBox::warning(this, "警告", "账号长度必须在4到16之间");
        return;
    } else if (common::containsInvalidChars(account.toStdString())) {
        QString illegalChars;
        for (const char c : common::illegalAlphabet) {
            illegalChars += QString(c) + " ";
        }
        QMessageBox::warning(this, "警告",
                             QString("用户名不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                             .arg(illegalChars));
        return;
    }

    if (password.size() < 4 || password.size() > 16) {
        QMessageBox::warning(this, "警告", "密码长度必须在4到16之间");
        return;
    } else if (common::containsInvalidChars(password.toStdString())) {
        QString illegalChars;
        for (const char c : common::illegalAlphabet) {
            illegalChars += QString(c) + " ";
        }
        QMessageBox::warning(this, "警告",
                             QString("密码不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                             .arg(illegalChars));
        return;
    }

    password = common::getSha256Hash(password);
    Client &client = Client::instance();

    // 断开旧连接防止重复绑定
    disconnect(&client, &Client::loginSuccess, this, nullptr);
    disconnect(&client, &Client::loginFailed, this, nullptr);

    client.login(account, password);
    // 注意：这里使用 [] 而不是 [bool]，因为 loginSuccess 不带参数
    QObject::connect(&client, &Client::loginSuccess, this, [this]() {
        QMessageBox::information(this, "提示", "登录成功！");
        emit loginSuccess();
        this->close();
    });
    QObject::connect(&client, &Client::loginFailed, this, [this](const QString &reason) {
        QMessageBox::critical(this, "错误", "登录失败：" + reason);
    });
}

void Login::registerBtn_clicked() {
    QString account = ui->accInput->text();
    QString password = ui->pwdInput->text();
    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "账号或密码不能为空");
        return;
    }
    if (account.size() < 4 || account.size() > 16) {
        QMessageBox::warning(this, "警告", "账号长度必须在4到16之间");
    } else if (common::containsInvalidChars(account.toStdString())) {
        QString illegalChars;
        for (const char c : common::illegalAlphabet) {
            illegalChars += QString(c) + " ";
        }
        QMessageBox::warning(this, "警告",
                             QString("用户名不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                           .arg(illegalChars));
        return;
    }
    if (password.size() < 4 || password.size() > 16) {
        QMessageBox::warning(this, "警告", "密码长度必须在4到16之间");
    } else if (common::containsInvalidChars(password.toStdString())) {
        QString illegalChars;
        for (const char c : common::illegalAlphabet) {
            illegalChars += QString(c) + " ";
        }
        QMessageBox::warning(this, "警告",
                             QString("密码不能包含非法字符。\n"
                                     "非法字符包括：\n%1")
                           .arg(illegalChars));
        return;
    }
    password = common::getSha256Hash(password);
    auto res = Client::instance().registerAccount(account, password);
    if (res) {
        emit registerSuccess();
    } else {
        QMessageBox::warning(this, "警告", "注册失败");
    }
}

void Login::changeHostBtn_clicked() {
    Client &client = Client::instance();
    // 创建输入框并设置默认值
    QLineEdit *ipEdit = new QLineEdit(client.getIP());
    QLineEdit *portEdit = new QLineEdit(client.getPort());
    // 创建标签和表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("IP 地址:", ipEdit);
    formLayout->addRow("端口号:", portEdit);
    // 创建按钮
    QPushButton *confirmBtn = new QPushButton("确认");
    QPushButton *cancelBtn = new QPushButton("取消");
    // 创建水平按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();  // 将按钮右对齐
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancelBtn);
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle("修改服务器");
    dialog.setLayout(mainLayout);
    dialog.resize(350, 120);
    // 连接按钮信号
    connect(confirmBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    // 显示对话框并处理返回值
    if (dialog.exec() == QDialog::Accepted) {
        QString ip = ipEdit->text();
        QString portStr = portEdit->text();
        bool ok;
        quint16 port = portStr.toUShort(&ok);
        if (!ok || port < 1 || port > 65535) {
            QMessageBox::warning(this, "错误", "请输入有效的端口号（1~65535）");
            return;
        }
        qDebug() << "用户确认：" << ip << ":" << port;
        client.setServer(ip, port);
        client.connectToServer();
    } else {
        qDebug() << "用户点击了取消";
    }
}

