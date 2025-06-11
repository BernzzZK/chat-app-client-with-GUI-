//
// Created by xwz on 25-6-11.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <QByteArray>
#include <QString>

class Buffer {
public:
    // 默认初始大小
    static const int kCheapPrepend = 8;
    static const int kInitialSize = 1024;

    // 构造函数
    explicit Buffer(int initialSize = kInitialSize);

    // 可读字节数
    int readableBytes() const;

    // 可写字节数
    int writableBytes() const;

    // 前面可预留字节数
    int prependableBytes() const;

    // 获取可读指针
    const char* peek() const;

    // 将已读指针前移 len 字节
    void retrieve(int len);

    // 取出所有可读内容并转为 QString，同时清空缓冲区
    QString retrieveAllAsString();

    // 取出所有可读内容并转为 QByteArray，同时清空缓冲区
    QByteArray retrieveAllAsQByteArray();

    // 添加数据到缓冲区
    void append(const char* data, int len);
    void append(const QByteArray& byteArray);

    // 确保有足够空间写入数据
    void ensureWritableBytes(int len);

    // 获取当前写指针
    char* beginWrite();

    // 获取当前写指针 const 版本
    const char* beginWrite() const;

    // 写指针前移 len 字节
    void hasWritten(int len);

private:
    QByteArray buffer_;
    int readerIndex_;
    int writerIndex_;

    // 私有辅助函数：调整缓冲区，腾出空间
    void makeSpace(int len);
};

#endif // BUFFER_H
