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
    m_sendTimer->start(1000);

    m_testTimer = new QTimer(this);
    connect(m_sendTimer, &QTimer::timeout, this, &Widget::on_testTimer_out, Qt::UniqueConnection);
    m_testTimer->start(100);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_testTimer_out()
{
    static int lc=0;
    static bool isOpen = true;
    lc++;
    if(lc>=8 && isOpen==true)
    {
        lc = 0;
        isOpen = false;
        ui->cb_ch1open->setChecked(true);
    }

    if(lc>=20 && isOpen==false)
    {
        lc = 0;
        isOpen = true;
        ui->cb_ch1open->setChecked(false);
    }
}

void Widget::on_sendTimer_out()
{
//    static int sendCount = 0;
//    sendCount ++;
//    if(sendCount>=20)
//    {
//        m_sendTimer->stop();
//    }
//    if(!m_tcpServer.getClientIds().isEmpty())
//    {
//        if(ui->cb_useUIsetting->isChecked())
//        {
//            handleUiSetting();
//        }
//        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1.join(","));
//    }
    if(!m_tcpServer_2.getClientIds().isEmpty())
    {
        m_tcpServer_2.sendMessage(m_tcpServer.getClientIds()[0],"SV,1,2,3,4,5,6,7,8,3,3,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26");
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
    for (int i=0;i<52;i++) {
        tcpData_1.append("N");
    }
    tcpData_1[0] = "RND";
    tcpData_1[1] = "1";
    tcpData_1[2] = "1";
    tcpData_1[23] = "1";
    tcpData_1[24] = "1";
    tcpData_1[51] = "\r\n";

    tcpData_1_2.clear();
    for (int i=0;i<59;i++) {
        tcpData_1_2.append("N");
    }
    tcpData_1_2[0] = "RND";
    tcpData_1_2[1] = "1";
    tcpData_1_2[2] = "1";
    tcpData_1_2[23] = "1";
    tcpData_1_2[24] = "1";
    tcpData_1_2[58] = "\r\n";
}


void Widget::on_pushButton_clicked()
{
//    tcpData_1[252] = ui->lineEdit_CH1_Lot->text();
//    tcpData_1[253] = ui->lineEdit_CH1_SLot->text();

//    tcpData_1[257] = ui->lineEdit_CH3_Lot->text();
//    tcpData_1[258] = ui->lineEdit_CH3_SLot->text();

//    tcpData_1[262] = ui->lineEdit_CH4_Lot->text();
//    tcpData_1[263] = ui->lineEdit_CH4_SLot->text();

//    tcpData_1[267] = ui->lineEdit_CH6_Lot->text();
//    tcpData_1[268] = ui->lineEdit_CH6_SLot->text();

//    tcpData_1[272] = ui->lineEdit_LLA_Lot->text();
//    tcpData_1[273] = ui->lineEdit_LLA_SLot->text();

//    tcpData_1[277] = ui->lineEdit_LLB_Lot->text();
//    tcpData_1[278] = ui->lineEdit_LLB_SLot->text();
}

void Widget::on_cb_ch1open_clicked(bool checked)
{
    tcpData_1_2[51] = "235850";                          //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235849";                          //CEID  关闭
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH1_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_CH1_SLot->text();   //SlotID
    tcpData_1_2[56] = "N";   //WaferID
    tcpData_1_2[57] = "N";   //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch3open_clicked(bool checked)
{
    tcpData_1_2[51] = "235856";  //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235855";  //CEID  关闭
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH3_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_CH3_SLot->text();   //SlotID
    tcpData_1_2[56] = "N";   //WaferID
    tcpData_1_2[57] = "N";   //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch4open_clicked(bool checked)
{
    tcpData_1_2[51] = "235862";  //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235861";  //CEID  关闭
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH4_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_CH4_SLot->text();   //SlotID
    tcpData_1_2[56] = "N";   //WaferID
    tcpData_1_2[57] = "N";   //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch6open_clicked(bool checked)
{
    tcpData_1_2[51] = "235868";  //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235867";  //CEID  关闭
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH6_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_CH6_SLot->text();   //SlotID
    tcpData_1_2[56] = "N";   //WaferID
    tcpData_1_2[57] = "N";   //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_LAopen_clicked(bool checked)
{
    tcpData_1_2[51] = "235842";  //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235841";  //CEID  关闭
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_LLA_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_LLA_SLot->text();   //SlotID
    tcpData_1_2[56] = "N";   //WaferID
    tcpData_1_2[57] = "N";   //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_LBopen_clicked(bool checked)
{
    tcpData_1_2[51] = "235844";                            //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235843";                        //CEID  关闭
    }
    tcpData_1_2[52] = "N";                            //RPTID
    tcpData_1_2[53] = "N";                            //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_LLB_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_LLB_SLot->text();  //SlotID
    tcpData_1_2[56] = "N";                            //WaferID
    tcpData_1_2[57] = "N";                            //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch1start_clicked(bool checked)
{
    tcpData_1_2[51] = "235847";                                   //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235848";                               //CEID  关闭
        ui->lineEdit_CH1_StepIndex->setText("0");            //结束归零
    }
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH1_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH1_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH1_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch3start_clicked(bool checked)
{
    tcpData_1_2[51] = "235853";  //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235854";  //CEID  关闭
        ui->lineEdit_CH3_StepIndex->setText("0");            //结束归零
    }
    tcpData_1_2[52] = "N";   //RPTID
    tcpData_1_2[53] = "N";   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH3_Lot->text();   //LotID
    tcpData_1_2[55] = ui->lineEdit_CH3_SLot->text();   //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH3_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch4start_clicked(bool checked)
{
    tcpData_1_2[51] = "235859";                                   //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235860";                               //CEID  关闭
        ui->lineEdit_CH4_StepIndex->setText("0");            //结束归零
    }
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH4_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH4_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH4_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_cb_ch6start_clicked(bool checked)
{
    tcpData_1_2[51] = "235865";                                   //CEID  默认为打开
    if(checked == false)
    {
        tcpData_1_2[51] = "235866";                               //CEID  关闭
        ui->lineEdit_CH6_StepIndex->setText("0");            //结束归零
    }
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH6_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH6_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH6_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_pushButton_Ch1_StepIndex_clicked()
{
    tcpData_1_2[51] = "235900";                                   //CEID
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH1_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH1_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH1_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_pushButton_Ch3_StepIndex_clicked()
{
    tcpData_1_2[51] = "235903";                                   //CEID
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH3_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH3_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH3_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_pushButton_Ch4_StepIndex_clicked()
{
    tcpData_1_2[51] = "235906";                                   //CEID
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH4_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH4_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH4_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}

void Widget::on_pushButton_Ch6_StepIndex6_clicked()
{
    tcpData_1_2[51] = "235909";                                   //CEID
    tcpData_1_2[52] = "N";                                   //RPTID
    tcpData_1_2[53] = "N";                                   //RecipeNameEQ
    tcpData_1_2[54] = ui->lineEdit_CH6_Lot->text();          //LotID
    tcpData_1_2[55] = ui->lineEdit_CH6_SLot->text();         //SlotID
    tcpData_1_2[56] = tcpData_1_2[54]+"#"+tcpData_1_2[55];   //WaferID
    tcpData_1_2[57] = ui->lineEdit_CH6_StepIndex->text();    //StepIndex
    if(!m_tcpServer.getClientIds().isEmpty())
    {
        m_tcpServer.sendMessage(m_tcpServer.getClientIds()[0],tcpData_1_2.join(","));
    }
}
