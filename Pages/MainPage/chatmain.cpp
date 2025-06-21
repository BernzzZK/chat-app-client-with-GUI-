#include "chatmain.h"
#include <QFile>
#include <QDebug>
#include <QMouseEvent>

#include "../../src/ChatDelegate.h"

ChatMain::ChatMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeChatWindow)
    , client(Client::instance())
{
    ui->setupUi(this);

    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);

    // 连接按钮
    connect(ui->minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);

    // 加载样式表
    QFile file(":/style/QSS/chatStyle.qss");
    if (!file.exists()) {
        qDebug() << "QSS file not found!";
    } else if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    ui->chatButton->setIcon(QIcon(":/icons/resources/chat.png"));
    ui->contactsButton->setIcon(QIcon(":/icons/resources/contacts.png"));
    ui->meButton->setIcon(QIcon(":/icons/resources/me.png"));
    ui->settingButton->setIcon(QIcon(":/icons/resources/settings.png"));

    ui->chatMessageList->setItemDelegate(new ChatDelegate(ui->chatMessageList));

    initChatMessages();
}

ChatMain::~ChatMain() {
    delete ui;
}

void ChatMain::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ChatMain::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void ChatMain::initChatMessages() const {
    // 对方消息（左对齐）
    QListWidgetItem* otherMessage = new QListWidgetItem("你好！");
    otherMessage->setData(Qt::UserRole, false); // 标记为对方消息
    ui->chatMessageList->addItem(otherMessage);
    // 我的消息（右对齐）
    QListWidgetItem* myMessage = new QListWidgetItem("收到");
    myMessage->setData(Qt::UserRole, true); // 标记为“我”的消息
    ui->chatMessageList->addItem(myMessage);
}
