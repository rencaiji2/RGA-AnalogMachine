#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initTcpData_1();
    m_tcpServer.startServer("127.0.0.1",6000);
    m_tcpServer_2.startServer("127.0.0.1",6001);
    m_sendTimer = new QTimer(this);
    connect(m_sendTimer, &QTimer::timeout, this, &Widget::on_sendTimer_out, Qt::UniqueConnection);
    m_sendTimer->start(100);
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
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1.join(","));
    }
    if(!m_tcpServer_2.getClientIds().isEmpty())
    {
        m_tcpServer_2.sendMessage(m_tcpServer.getClientIds()[0],"SV,1,2,3,4,5,6,7,8,3,3,11,12,13,14,15,16,17,18");
    }
}

void Widget::handleUiSetting()
{

    tcpData_1[51] = QString::number(ui->cb_ch1start->isChecked());
    tcpData_1[55] = QString::number(ui->cb_ch3start->isChecked());
    tcpData_1[59] = QString::number(ui->cb_ch4start->isChecked());
    tcpData_1[63] = QString::number(ui->cb_ch6start->isChecked());

    tcpData_1[75] = QString::number(ui->cb_ch1open->isChecked());
    tcpData_1[77] = QString::number(ui->cb_ch3open->isChecked());
    tcpData_1[79] = QString::number(ui->cb_ch4open->isChecked());
    tcpData_1[81] = QString::number(ui->cb_ch6open->isChecked());
    tcpData_1[83] = QString::number(ui->cb_LAopen->isChecked());
    tcpData_1[85] = QString::number(ui->cb_LBopen->isChecked());
}

void Widget::initTcpData_1()
{
    tcpData_1.clear();
    for (int i=0;i<300;i++) {
        tcpData_1.append("N");
    }
    tcpData_1[0] = "RND";
    tcpData_1[1] = "1";
    tcpData_1[2] = "1";
    tcpData_1[23] = "1";
    tcpData_1[24] = "1";
}


void Widget::on_pushButton_clicked()
{
    tcpData_1[252] = ui->lineEdit_CH1_Lot->text();
    tcpData_1[253] = ui->lineEdit_CH1_SLot->text();

    tcpData_1[257] = ui->lineEdit_CH3_Lot->text();
    tcpData_1[258] = ui->lineEdit_CH3_SLot->text();

    tcpData_1[262] = ui->lineEdit_CH4_Lot->text();
    tcpData_1[263] = ui->lineEdit_CH4_SLot->text();

    tcpData_1[267] = ui->lineEdit_CH6_Lot->text();
    tcpData_1[268] = ui->lineEdit_CH6_SLot->text();

    tcpData_1[272] = ui->lineEdit_LLA_Lot->text();
    tcpData_1[273] = ui->lineEdit_LLA_SLot->text();

    tcpData_1[277] = ui->lineEdit_LLB_Lot->text();
    tcpData_1[278] = ui->lineEdit_LLB_SLot->text();
}
