//
// Created by xwz on 25-6-13.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Register.h" resolved

#include "register.h"
#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>
#include <qscreen.h>
#include "Client.h"
#include "Common.h"
#include "ui_Register.h"


Register::Register(QWidget *parent) : QDialog(parent), ui(new Ui::Register), client(Client::instance()) {
    ui->setupUi(this);

    // 设置无边框窗口
    // setWindowFlags(Qt::FramelessWindowHint);

    // 设置窗口大小和标题
    resize(400, 300);
    setWindowTitle("注册");

    // 居中显示
    centerWindow();

    // 加载样式表
    QFile file(":/style/QSS/registerStyle.qss");
    if (!file.exists()) {
        qDebug() << "QSS file not found!";
    } else if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    // 连接按钮信号
    connect(ui->registerBtn, &QPushButton::clicked, this, &Register::registerBtn_clicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, [this]() {
        emit registerCancel();
        this->close();
    });

    // 初始化鼠标拖动变量
    m_dragging = false;
}

Register::~Register() {
    delete ui;
}

void Register::registerBtn_clicked() {
    QString account = ui->reg_acc_input->text();
    QString password = ui->reg_pwd_input->text();
    QString confirmPassword = ui->reg_pwd2_input->text();
    auto *widget = new QWidget;
    if (!common::validateAccount(widget, account) || !common::validatePassword(widget, password) || !common::validatePassword(widget, confirmPassword)) {
        delete widget;
        return;
    }
    if (password != confirmPassword) {
        QMessageBox::warning(widget, "警告", "两次输入的密码不一致");
        delete widget;
        return;
    }
    std::string pwd = common::encrypt(password.toStdString());
    if (client.isConnected()) {
        client.registerAccount(account, pwd);
    } else {
        QMessageBox::warning(widget, "警告", "未连接到服务器");
    }
    connect(&client, &Client::registerSuccess, this, [this, widget]() {
        widget->deleteLater();
        this->close();
    });
}

void Register::centerWindow() {
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void Register::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Register::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void Register::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    m_dragging = false;
}
