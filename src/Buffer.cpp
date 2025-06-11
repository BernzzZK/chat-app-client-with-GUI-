//
// Created by xwz on 25-6-11.
//

#include "Buffer.h"
#include <QDebug>

Buffer::Buffer(int initialSize)
    : buffer_(initialSize + kCheapPrepend, '\0'), readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend)
{
}

int Buffer::readableBytes() const {
    return writerIndex_ - readerIndex_;
}

int Buffer::writableBytes() const {
    return buffer_.size() - writerIndex_;
}

int Buffer::prependableBytes() const {
    return readerIndex_;
}

const char* Buffer::peek() const {
    return buffer_.data() + readerIndex_;
}

void Buffer::retrieve(int len) {
    if (len < readableBytes()) {
        readerIndex_ += len;
    } else {
        // 全部读完，重置索引
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
}

QString Buffer::retrieveAllAsString() {
    QString result = QString::fromUtf8(buffer_.data() + readerIndex_, readableBytes());
    retrieve(readableBytes());
    return result;
}

QByteArray Buffer::retrieveAllAsQByteArray() {
    QByteArray result(buffer_.data() + readerIndex_, readableBytes());
    retrieve(readableBytes());
    return result;
}

void Buffer::append(const char* data, int len) {
    if (len == 0) return;
    ensureWritableBytes(len);
    memcpy(beginWrite(), data, len);
    hasWritten(len);
}

void Buffer::append(const QByteArray& byteArray) {
    append(byteArray.data(), byteArray.size());
}

void Buffer::ensureWritableBytes(int len) {
    if (writableBytes() < len) {
        // 需要扩容
        int newCapacity = buffer_.size() * 2 + len;
        QByteArray newBuffer(newCapacity + kCheapPrepend, '\0');
        int readable = readableBytes();
        memcpy(newBuffer.data() + kCheapPrepend, peek(), readable);
        buffer_ = newBuffer;
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend + readable;
    }
}

char* Buffer::beginWrite() {
    return buffer_.data() + writerIndex_;
}

const char* Buffer::beginWrite() const {
    return buffer_.data() + writerIndex_;
}

void Buffer::hasWritten(int len) {
    writerIndex_ += len;
}
