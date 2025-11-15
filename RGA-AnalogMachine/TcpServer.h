#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QObject>
#include <QVector>
#include "ClientSocket.h" // 引入独立的ClientSocket头文件

class TcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    // 启动服务器
    bool startServer(const QString &ip, quint16 port);

    // 停止服务器
    void stopServer();

    // 发送消息给指定客户端
    bool sendMessage(int clientId, const QString &message);

    // 广播消息给所有客户端
    void broadcastMessage(const QString &message);

    // 获取连接的客户端数量
    int getClientCount() const;

    // 获取客户端ID列表
    QVector<int> getClientIds() const;

signals:
    // 客户端连接信号
    void clientConnected(int clientId, const QString &clientIp, quint16 clientPort);

    // 客户端断开信号
    void clientDisconnected(int clientId);

    // 收到客户端消息信号
    void messageReceived(int clientId, const QString &message, const QString &clientIp);

    // 消息发送失败信号
    void sendFailed(int clientId, const QString &error);

    // 服务器错误信号
    void serverError(const QString &error);

protected:
    // 新连接到达时调用
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QVector<ClientSocket*> m_clients;
    int m_nextClientId;
};

#endif // TCPSERVER_H
