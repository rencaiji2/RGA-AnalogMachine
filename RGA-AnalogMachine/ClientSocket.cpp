#include "ClientSocket.h"
#include <QDebug>
#include <QDateTime>

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
    QByteArray body;
    body.append('\x01'); // List, 1-byte length
    body.append('\x02'); // 2 items
    body.append('\xB1'); // U4, 1-byte length (与Host编码一致)
    body.append('\x04'); // 4 bytes
    body.append(QByteArray("\x00\x00\x00\x0B", 4)); // value 11
    body.append('\xB1'); // U4, 1-byte length
    body.append('\x04'); // 4 bytes
    body.append(QByteArray("\x00\x00\x00\x0C", 4)); // value 12
    return body;
}

static QByteArray makeSecsIIBodyForS1F14()
{
    // S1F14 Establish Communication Request ACK:
    // <L[2]>
    //   <B[1]> 0                         // COMMACK = 0 (OK)
    //   <L[2]>
    //     <A[2]> "10"                    // MDLN
    //     <A[1]> "0"                     // SOFTREV
    //   >
    // >
    QByteArray body;
    body.append('\x01'); // List, 1-byte length
    body.append('\x02'); // 2 items
    // COMMACK = Binary 0
    body.append('\x21'); // Binary, 1-byte length (标准SECS-II格式码 0o10)
    body.append('\x01'); // 1 byte
    body.append('\x00'); // value 0
    // MDLN + SOFTREV list
    body.append('\x01'); // List, 1-byte length
    body.append('\x02'); // 2 items
    body.append('\x41'); // ASCII, 1-byte length
    body.append('\x02'); // 2 bytes
    body.append("10");
    body.append('\x41'); // ASCII, 1-byte length
    body.append('\x01'); // 1 byte
    body.append("0");
    return body;
}

// ---------------------------------------------------------------------------
// SECS-II 编码辅助函数
// ---------------------------------------------------------------------------

// 构造 <B[1]> val
static QByteArray makeBinary1(quint8 val)
{
    QByteArray b;
    b.append('\x21'); // Binary, 1-byte length (标准SECS-II格式码 0o10)
    b.append('\x01');
    b.append((char)val);
    return b;
}

// 构造 <A "str">
static QByteArray makeAscii(const QString &str)
{
    QByteArray b;
    QByteArray utf8 = str.toUtf8();
    b.append('\x41'); // ASCII, 1-byte length
    b.append((char)utf8.size());
    b.append(utf8);
    return b;
}

// ---------------------------------------------------------------------------
// 新增 SxFy 回复消息体
// ---------------------------------------------------------------------------

// S1F16 Offline Acknowledge: <B[1]> 0 (OFACK=0)
static QByteArray makeSecsIIBodyForS1F16()
{
    return makeBinary1(0x00);
}

// S1F18 Online Acknowledge: <B[1]> 0 (ONLACK=0)
static QByteArray makeSecsIIBodyForS1F18()
{
    return makeBinary1(0x00);
}

// S2F18 Date and Time Data: <A "YYYYMMDDHHMMSS">
static QByteArray makeSecsIIBodyForS2F18()
{
    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    return makeAscii(timeStr);
}

// S2F32 Date and Time Set Acknowledge: <B[1]> 0 (TIACK=0)
static QByteArray makeSecsIIBodyForS2F32()
{
    return makeBinary1(0x00);
}

// S2F34 Define Report Acknowledge: <B[1]> 0 (DRACK=0)
static QByteArray makeSecsIIBodyForS2F34()
{
    return makeBinary1(0x00);
}

// S2F36 Link Event Report Acknowledge: <B[1]> 0 (LRACK=0)
static QByteArray makeSecsIIBodyForS2F36()
{
    return makeBinary1(0x00);
}

// S2F38 Enable/Disable Event Report Acknowledge: <B[1]> 0 (ERACK=0)
static QByteArray makeSecsIIBodyForS2F38()
{
    return makeBinary1(0x00);
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
            quint8 sType = 0xFF;
            if ((byte3 & 0x7F) != 0) {
                sType = byte3 & 0x7F;
            } else if (byte5 != 0) {
                sType = byte5;
            }

            // Select.req (SType=1) → Select.rsp (SType=2)
            if (sType == 0x01) {
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
            }
            // Linktest.req (SType=5) → Linktest.rsp (SType=6)
            else if (sType == 0x05) {
                qDebug() << "收到 Linktest.req，回复 Linktest.rsp";
                QByteArray rspHeader = msg.left(10);
                if (byte5 == 0x05) {
                    rspHeader[5] = 0x06;
                } else {
                    rspHeader[3] = 0x06;
                }
                QByteArray packet = makeHsmsPacket(rspHeader, QByteArray());
                write(packet);
                qDebug() << "发送 Linktest.rsp(hex):" << packet.toHex(' ');
            }
            // Deselect.req (SType=3) → Deselect.rsp (SType=4)
            else if (sType == 0x03) {
                qDebug() << "收到 Deselect.req，回复 Deselect.rsp";
                QByteArray rspHeader = msg.left(10);
                if (byte5 == 0x03) {
                    rspHeader[5] = 0x04;
                } else {
                    rspHeader[3] = 0x04;
                }
                QByteArray packet = makeHsmsPacket(rspHeader, QByteArray());
                write(packet);
                qDebug() << "发送 Deselect.rsp(hex):" << packet.toHex(' ');
            }
            // Separate.req (SType=9) — 不需要回复
            else if (sType == 0x09) {
                qDebug() << "收到 Separate.req，对端主动断开";
            }
            else {
                qDebug() << "收到其他 HSMS 控制消息, SType=" << sType;
            }
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
        bool handled = true;

        // --- S1 通信状态 ---
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
        else if (stream == 1 && function == 15) {
            replyHeader[3] = 0x10; // S1F16
            replyBody = makeSecsIIBodyForS1F16();
        }
        else if (stream == 1 && function == 17) {
            replyHeader[3] = 0x12; // S1F18
            replyBody = makeSecsIIBodyForS1F18();
        }
        // --- S2 设备控制/数据采集 ---
        else if (stream == 2 && function == 17) {
            replyHeader[3] = 0x12; // S2F18 时间查询回复
            replyBody = makeSecsIIBodyForS2F18();
        }
        else if (stream == 2 && function == 31) {
            replyHeader[3] = 0x20; // S2F32 时间设置回复
            replyBody = makeSecsIIBodyForS2F32();
        }
        else if (stream == 2 && function == 33) {
            replyHeader[3] = 0x22; // S2F34 定义报告回复
            replyBody = makeSecsIIBodyForS2F34();
        }
        else if (stream == 2 && function == 35) {
            replyHeader[3] = 0x24; // S2F36 关联事件报告回复
            replyBody = makeSecsIIBodyForS2F36();
        }
        else if (stream == 2 && function == 37) {
            replyHeader[3] = 0x26; // S2F38 使能事件报告回复
            replyBody = makeSecsIIBodyForS2F38();
        }
        else {
            handled = false;
            qDebug() << "暂未实现 S" << stream << "F" << function << " 的自动回复";
        }

        if (!handled) {
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
