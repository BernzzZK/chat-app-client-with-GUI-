//
// Created by xwz on 25-6-18.
//

#include "ChatDelegate.h"
#include <QPainter>
#include <QPainterPath>

ChatDelegate::ChatDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void ChatDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    bool isMyMessage = index.data(Qt::UserRole).toBool();

    if (isMyMessage) {
        opt.backgroundBrush = QBrush(Qt::green);
        opt.palette.setColor(QPalette::Text, Qt::black);
        opt.displayAlignment = Qt::AlignRight;
    } else {
        opt.backgroundBrush = QBrush(Qt::gray);
        opt.palette.setColor(QPalette::Text, Qt::black);
        opt.displayAlignment = Qt::AlignLeft;
    }

    QStyledItemDelegate::paint(painter, opt, index);
}
