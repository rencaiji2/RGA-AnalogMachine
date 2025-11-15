#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <QObject>

class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
    explicit ClientSocket(int id, QObject *parent = nullptr);
    ~ClientSocket();

    int getId() const { return m_id; }

signals:
    void dataReceived(const QString &message);
    void disconnectedFromHost();

protected slots:
    virtual void onReadyRead();
    virtual void onDisconnected();

private:
    int m_id;
};

#endif // CLIENTSOCKET_H
