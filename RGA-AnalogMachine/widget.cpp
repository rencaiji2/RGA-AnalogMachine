#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_tcpServer.startServer("127.0.0.1",6000);
    m_sendTimer = new QTimer(this);
    connect(m_sendTimer, &QTimer::timeout, this, &Widget::on_sendTimer_out, Qt::UniqueConnection);
    m_sendTimer->start(1000);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_sendTimer_out()
{
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],m_agreementZXJC.Splicing());
    }
}

