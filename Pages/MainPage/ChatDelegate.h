//
// Created by xwz on 25-6-18.
//

#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H

#include <QStyledItemDelegate>

class ChatDelegate : public QStyledItemDelegate {
public:

    explicit ChatDelegate(QObject* parent = nullptr); // 使用 QObject* 作为父对象
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

};

#endif //CHATDELEGATE_H
