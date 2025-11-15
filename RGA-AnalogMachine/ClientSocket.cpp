#include "ClientSocket.h"
#include <QDebug>

ClientSocket::ClientSocket(int id, QObject *parent)
    : QTcpSocket(parent), m_id(id)
{
    connect(this, &QTcpSocket::readyRead, this, &ClientSocket::onReadyRead);
    connect(this, &QTcpSocket::disconnected, this, &ClientSocket::onDisconnected);
}

ClientSocket::~ClientSocket()
{
    // 析构函数，可以在这里添加清理工作
}

void ClientSocket::onReadyRead()
{
    QByteArray data = readAll();
    QString message = QString::fromUtf8(data);
    emit dataReceived(message);
}

void ClientSocket::onDisconnected()
{
    emit disconnectedFromHost();
}
