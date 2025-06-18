#ifndef CHATMAIN_H
#define CHATMAIN_H

#include <QWidget>

#include "Client.h"
#include "ui_ChatMain.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WeChatWindow; }
QT_END_NAMESPACE

class ChatMain : public QWidget {
    Q_OBJECT

public:
    explicit ChatMain(QWidget *parent = nullptr);
    ~ChatMain();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void initChatMessages() const;

private:
    Ui::WeChatWindow *ui;
    QPoint dragPosition;

    Client &client;
};

#endif // CHATMAIN_H