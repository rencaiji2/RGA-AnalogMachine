#include "ClientSocket.h"
#include <QDebug>

ClientSocket::ClientSocket(int id, QObject *parent)
    : QTcpSocket(parent), m_id(id)
{
    qDebug() << "ClientSocket 创建, ID:" << id;
    connect(this, &QTcpSocket::readyRead, this, &ClientSocket::onReadyRead);
    connect(this, &QTcpSocket::disconnected, this, &ClientSocket::onDisconnected);
}

ClientSocket::~ClientSocket()
{
}

static QByteArray makeHsmsPacket(const QByteArray &header, const QByteArray &body)
{
    QByteArray packet;
    quint32 totalLen = header.size() + body.size();
    packet.append((char)((totalLen >> 24) & 0xFF));
    packet.append((char)((totalLen >> 16) & 0xFF));
    packet.append((char)((totalLen >> 8) & 0xFF));
    packet.append((char)(totalLen & 0xFF));
    packet.append(header);
    packet.append(body);
    return packet;
}

static QByteArray makeSecsIIBodyForS1F2()
{
    // S1F2 Online Data: empty list
    return QByteArray("\x01\x00", 2);
}

static QByteArray makeSecsIIBodyForS1F4()
{
    // S1F4 Selected Equipment Status Data: <L> <U4 11> <U4 12> </L>
    // U4 format byte = 0x40 | (7<<2) | 0 = 0x5C, length 1 byte
    QByteArray body;
    body.append('\x01'); // List, 1-byte length
    body.append('\x02'); // 2 items
    body.append('\x5C'); // U4, 1-byte length
    body.append('\x04'); // 4 bytes
    body.append(QByteArray("\x00\x00\x00\x0B", 4)); // value 11
    body.append('\x5C'); // U4, 1-byte length
    body.append('\x04'); // 4 bytes
    body.append(QByteArray("\x00\x00\x00\x0C", 4)); // value 12
    return body;
}

static QByteArray makeSecsIIBodyForS1F14()
{
    // S1F14 Establish Communication Request ACK:
    // <L>
    //   <B>0</B>                         // COMMACK
    //   <L><A>"10"</A></L>               // MDLN
    // </L>
    QByteArray body;
    body.append('\x01'); // List, 1-byte length
    body.append('\x02'); // 2 items
    // COMMACK = Binary 0
    body.append('\x44'); // Binary, 1-byte length
    body.append('\x01'); // 1 byte
    body.append('\x00'); // value 0
    // MDLN list
    body.append('\x01'); // List, 1-byte length
    body.append('\x01'); // 1 item
    body.append('\x41'); // ASCII, 1-byte length
    body.append('\x02'); // 2 bytes
    body.append("10");
    return body;
}

void ClientSocket::onReadyRead()
{
    QByteArray data = readAll();
    qDebug() << "收到原始数据(hex):" << data.toHex(' ') << "长度:" << data.size();

    if (data.isEmpty()) {
        return;
    }

    m_buffer.append(data);

    const int HSMS_MIN_LEN = 10;
    const quint32 MAX_MSG_LEN = 1024 * 1024;

    while (m_buffer.size() >= 4 + HSMS_MIN_LEN) {
        quint32 msgLen = ((quint8)m_buffer[0] << 24) |
                         ((quint8)m_buffer[1] << 16) |
                         ((quint8)m_buffer[2] << 8)  |
                         ((quint8)m_buffer[3]);

        if (msgLen > MAX_MSG_LEN || msgLen < HSMS_MIN_LEN) {
            qDebug() << "异常 HSMS 长度，清空缓冲区:" << msgLen;
            m_buffer.clear();
            break;
        }

        if ((quint32)m_buffer.size() < 4 + msgLen) {
            qDebug() << "数据不完整，等待更多数据. 已有:" << m_buffer.size() << "需要:" << 4 + msgLen;
            break;
        }

        QByteArray msg = m_buffer.mid(4, msgLen);
        m_buffer.remove(0, 4 + msgLen);

        qDebug() << "解析到 HSMS 消息(hex):" << msg.toHex(' ');

        if (msg.size() < 10) {
            qDebug() << "HSMS 消息头长度不足 10";
            continue;
        }

        quint16 sessionId = ((quint8)msg[0] << 8) | (quint8)msg[1];
        quint8 byte2 = (quint8)msg[2];
        quint8 byte3 = (quint8)msg[3];
        quint8 byte5 = (quint8)msg[5];
        QByteArray sysBytes = msg.mid(6, 4);

        qDebug() << "解析 HSMS 头: SessionID="
                 << QString("0x%1").arg(sessionId, 4, 16, QChar('0'))
                 << "Byte2=" << QString("0x%1").arg(byte2, 2, 16, QChar('0'))
                 << "Byte3=" << QString("0x%1").arg(byte3, 2, 16, QChar('0'))
                 << "Byte5=" << QString("0x%1").arg(byte5, 2, 16, QChar('0'));

        // 控制消息：Session ID 为 0xFFFF
        if (sessionId == 0xFFFF) {
            // 兼容两种 HSMS 头布局：
            // 1) 标准：SType 在 byte3（Select.req=0x01）
            // 2) 该包装库：SType 在 byte5
            bool isSelectReq = ((byte3 & 0x7F) == 0x01) || (byte5 == 0x01);
            if (isSelectReq) {
                qDebug() << "收到 Select.req，回复 Select.rsp";
                QByteArray rspHeader = msg.left(10);
                if ((byte3 & 0x7F) == 0x01) {
                    rspHeader[3] = (char)((byte3 & 0x80) | 0x02);
                }
                if (byte5 == 0x01) {
                    rspHeader[5] = 0x02;
                }
                QByteArray packet = makeHsmsPacket(rspHeader, QByteArray());
                write(packet);
                qDebug() << "发送 Select.rsp(hex):" << packet.toHex(' ');
                continue;
            }

            qDebug() << "收到其他 HSMS 控制消息";
            continue;
        }

        // 数据消息：从 byte2/byte3 解析 stream/function
        quint8 stream = byte2 & 0x7F;
        quint8 function = byte3;
        bool wBit = (byte2 & 0x80) != 0;

        qDebug() << "收到 SECS 数据消息 S" << stream << "F" << function << "WBit=" << wBit;

        if (!wBit) {
            // 不需要回复的数据消息
            continue;
        }

        QByteArray replyHeader(10, '\0');
        replyHeader[0] = msg[0];
        replyHeader[1] = msg[1];
        replyHeader[2] = stream;               // reply 不带 R-bit
        replyHeader[4] = '\0';
        replyHeader[5] = '\0';
        replyHeader[6] = sysBytes[0];
        replyHeader[7] = sysBytes[1];
        replyHeader[8] = sysBytes[2];
        replyHeader[9] = sysBytes[3];

        QByteArray replyBody;
        if (stream == 1 && function == 1) {
            replyHeader[3] = 0x02; // S1F2
            replyBody = makeSecsIIBodyForS1F2();
        }
        else if (stream == 1 && function == 3) {
            replyHeader[3] = 0x04; // S1F4
            replyBody = makeSecsIIBodyForS1F4();
        }
        else if (stream == 1 && function == 13) {
            replyHeader[3] = 0x0E; // S1F14
            replyBody = makeSecsIIBodyForS1F14();
        }
        else {
            qDebug() << "暂未实现 S" << stream << "F" << function << " 的自动回复";
            continue;
        }

        QByteArray packet = makeHsmsPacket(replyHeader, replyBody);
        write(packet);
        qDebug() << "发送 S" << stream << "F" << (function + 1)
                 << " 回复(hex):" << packet.toHex(' ');
    }

    emit dataReceived(QString::fromUtf8(data));
}

void ClientSocket::onDisconnected()
{
    qDebug() << "ClientSocket 断开, ID:" << m_id;
    emit disconnectedFromHost();
}
