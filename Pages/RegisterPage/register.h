//
// Created by xwz on 25-6-13.
//

#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>


class Client;
QT_BEGIN_NAMESPACE
namespace Ui { class Register; }
QT_END_NAMESPACE

class Register : public QDialog {
Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register() override;
    void centerWindow();

private:
    Ui::Register *ui;// 居中窗口
    bool m_dragging;                         // 拖动状态
    QPoint m_dragPosition;

    Client &client;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;// 拖动起始位置

private slots:
    void registerBtn_clicked();
    signals:
        void registerCancel();
};


#endif //REGISTER_H
