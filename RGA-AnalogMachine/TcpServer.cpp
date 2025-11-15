#include "TcpServer.h"
#include <QDebug>
#include <QHostAddress>

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent), m_nextClientId(1)
{
}

TcpServer::~TcpServer()
{
    stopServer();
}

bool TcpServer::startServer(const QString &ip, quint16 port)
{
    QHostAddress address(ip);
    if (!listen(address, port)) {
        emit serverError("无法启动服务器: " + errorString());
        return false;
    }
    qDebug() << "TCP服务器已启动，IP:" << ip << ", 端口:" << port;
    return true;
}

void TcpServer::stopServer()
{
    // 先关闭所有客户端连接
    foreach (ClientSocket *client, m_clients) {
        client->disconnectFromHost();
    }

    // 停止监听
    close();
    qDebug() << "TCP服务器已停止";
}

bool TcpServer::sendMessage(int clientId, const QString &message)
{
    foreach (ClientSocket *client, m_clients) {
        if (client->getId() == clientId) {
            if (client->state() == QAbstractSocket::ConnectedState) {
                client->write(message.toUtf8());
                return true;
            } else {
                emit sendFailed(clientId, "客户端未连接");
                return false;
            }
        }
    }
    emit sendFailed(clientId, "客户端ID不存在");
    return false;
}

void TcpServer::broadcastMessage(const QString &message)
{
    foreach (ClientSocket *client, m_clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(message.toUtf8());
        }
    }
}

int TcpServer::getClientCount() const
{
    return m_clients.size();
}

QVector<int> TcpServer::getClientIds() const
{
    QVector<int> ids;
    foreach (const ClientSocket *client, m_clients) {
        ids.append(client->getId());
    }
    return ids;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    ClientSocket *clientSocket = new ClientSocket(m_nextClientId++, this);

    if (clientSocket->setSocketDescriptor(socketDescriptor)) {
        // 连接信号
        connect(clientSocket, &ClientSocket::dataReceived, this, [=](const QString &message) {
            emit messageReceived(clientSocket->getId(), message, clientSocket->peerAddress().toString());
        });

        connect(clientSocket, &ClientSocket::disconnectedFromHost, this, [=]() {
            int clientId = clientSocket->getId();
            m_clients.removeAll(clientSocket);
            clientSocket->deleteLater();
            emit clientDisconnected(clientId);
            qDebug() << "客户端断开连接，ID:" << clientId;
        });

        m_clients.append(clientSocket);
        emit clientConnected(clientSocket->getId(),
                            clientSocket->peerAddress().toString(),
                            clientSocket->peerPort());
        qDebug() << "新客户端连接，ID:" << clientSocket->getId()
                << "IP:" << clientSocket->peerAddress().toString()
                << "端口:" << clientSocket->peerPort();
    } else {
        delete clientSocket;
        emit serverError("无法初始化客户端套接字");
    }
}
