//
// Created by xwz on 25-6-10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Login.h" resolved

#include "login.h"
#include "ui_Login.h"
#include "../../src/Common.h"
#include "../RegisterPage/register.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFormLayout>
#include <QFile>
#include <QGraphicsDropShadowEffect>

#include "../MainPage/chatmain.h"

Login::Login(QWidget *parent) : QWidget(parent), ui(new Ui::Login), client(Client::instance()) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    QFile file(":/style/QSS/loginStyle.qss");
    if (!file.exists()) {
        qDebug() << "QSS file not found!";
    } else if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
    ui->rememberMe->setChecked(client.getRememberMe());
    if (client.getRememberMe()) {
        ui->accInput->setText(client.getAccount());
        ui->pwdInput->setText(QString::fromStdString(common::decrypt(client.getPassword().toStdString())));
        if (client.getAutoLogin()) {
            ui->autoLogin->setChecked(true);
            timer = new QTimer(this);
            timer->setSingleShot(true);
            timer->start(2500);
            connect(timer, &QTimer::timeout, this, &Login::autoLogin);
        }
    }

    ui->forgetPwd->setCursor(Qt::PointingHandCursor); // 鼠标悬停变成手型
    ui->forgetPwd->installEventFilter(this);         // 安装事件过滤器

    // 连接按钮信号
    connect(ui->minimizeBtn, &QPushButton::clicked, this, &Login::showMinimized);
    connect(ui->closeBtn, &QPushButton::clicked, this, &Login::close);

    QFont btnFont("Microsoft YaHei", 12, QFont::Bold);
    ui->loginBtn->setFont(btnFont);
    ui->registerBtn->setFont(btnFont);

    ui->changeHostBtn->setText("切换服务器");

    initClientConnection();
    connect(ui->loginBtn, &QPushButton::clicked, this, &Login::loginBtn_clicked);
    connect(ui->registerBtn, &QPushButton::clicked, this, &Login::registerBtn_clicked);
    connect(ui->changeHostBtn, &QPushButton::clicked, this, &Login::changeHostBtn_clicked);

    connect(&client, &Client::loginSuccess, this, [this]() {
        QMessageBox::information(this, "提示", "登录成功！");
        loginSusses();
    });
    connect(&client, &Client::loginFailed, this, [this](const QString &reason) {
        QMessageBox::critical(this, "错误", "登录失败：" + reason);
        ui->accInput->clear();
        ui->pwdInput->clear();
    });

    connect(&client, &Client::registerSuccess, this, [this]() {
        QMessageBox::information(this, "提示", "注册成功");
    });
    connect(&client, &Client::registerFailed, this, [this](const QString &reason) {
        QMessageBox::critical(this, "错误", "注册失败：" + reason);
        ui->accInput->clear();
        ui->pwdInput->clear();
    });

    connect(ui->autoLogin, &QCheckBox::stateChanged, this, [this]() {
        if (ui->autoLogin->isChecked()) {
            client.setAutoLogin(true);
            client.setRememberMe(true);
        } else {
            client.setAutoLogin(false);
        }
    });

    connect(ui->rememberMe, &QCheckBox::stateChanged, this, [this]() {
        if (ui->rememberMe->isChecked()) {
            client.setRememberMe(true);
        } else {
            client.setRememberMe(false);
        }
    });
}

Login::~Login() {
    delete ui;
}

void Login::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->pos().y() < 30) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Login::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void Login::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_dragging = false;
}

void Login::initClientConnection() {
    ui->labHost->setText("连接中...");
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
    if (client.isLogin()) return;
    QString account = ui->accInput->text();
    QString password = ui->pwdInput->text();

    if (!common::validateAccount(this, account) || !common::validatePassword(this, password)) {
        return;
    }
    std::string pwd = common::encrypt(password.toStdString());
    if (client.isConnected()) {
        client.login(account, pwd);
        connect(&client, &Client::loginSuccess, this, [this, account, pwd]() {
            if (ui->rememberMe->isChecked()) {
                client.setAccount(account);
                client.setPassword(QString::fromStdString(pwd));
            }
        });
    } else {
        QMessageBox::warning(this, "警告", "未连接到服务器");
    }
}

void Login::registerBtn_clicked() {
    this->hide();

    Register* registerPage = new Register(this);

    connect(&client, &Client::registerSuccess, this, [this, registerPage]() {
        this->show();
        registerPage->close();
    });

    connect(registerPage, &Register::registerCancel, this, [this, registerPage]() {
        this->show();
        registerPage->close();
    });

    registerPage->setWindowModality(Qt::ApplicationModal);
    registerPage->resize(400, 300);
    registerPage->setWindowTitle("注册");
    registerPage->show();
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
        client.setServer(ip, port);
        client.connectToServer();
    }
}

bool Login::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->forgetPwd) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                emit forgetPwd_clicked(); // 触发对应的槽函数
                return true;              // 表示事件已被处理
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Login::forgetPwd_clicked() {
    QMessageBox::information(this, "提示", "忘记密码功能尚未实现");
}

void Login::autoLogin() {
    loginBtn_clicked();
}

void Login::loginSusses() {
    this->close();
    auto chatWindow = new ChatMain();
    chatWindow->show();
}

