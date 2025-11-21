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
        if(ui->cb_useUIsetting->isChecked())
        {
            handleUiSetting();
        }
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],m_agreementZXJC.Splicing());
    }
}

void Widget::handleUiSetting()
{
    m_agreementZXJC.CH1ReceipeStart_3 = QString::number(ui->cb_ch1start->isChecked());
    m_agreementZXJC.CH3ReceipeStart_5 = QString::number(ui->cb_ch3start->isChecked());
    m_agreementZXJC.CH4ReceipeStart_7 = QString::number(ui->cb_ch4start->isChecked());
    m_agreementZXJC.CH6ReceipeStart_9 = QString::number(ui->cb_ch6start->isChecked());

    m_agreementZXJC.Ch1Slot_122 = QString::number(ui->cb_ch1open->isChecked());
    m_agreementZXJC.Ch3Slot_123 = QString::number(ui->cb_ch3open->isChecked());
    m_agreementZXJC.Ch4Slot_124 = QString::number(ui->cb_ch4open->isChecked());
    m_agreementZXJC.Ch6Slot_125 = QString::number(ui->cb_ch6open->isChecked());
}

