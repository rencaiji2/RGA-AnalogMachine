#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "TcpServer.h"
#include <QTimer>
#include "PublicStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_cb_ch1open_clicked(bool checked);

    void on_cb_ch3open_clicked(bool checked);

    void on_cb_ch4open_clicked(bool checked);

    void on_cb_ch6open_clicked(bool checked);

    void on_cb_LAopen_clicked(bool checked);

    void on_cb_LBopen_clicked(bool checked);

    void on_cb_ch1start_clicked(bool checked);

    void on_cb_ch3start_clicked(bool checked);

    void on_cb_ch4start_clicked(bool checked);

    void on_cb_ch6start_clicked(bool checked);

    void on_pushButton_Ch1_StepIndex_clicked();

    void on_pushButton_Ch3_StepIndex_clicked();

    void on_pushButton_Ch4_StepIndex_clicked();

    void on_pushButton_Ch6_StepIndex6_clicked();

private:
    TcpServer m_tcpServer;
    TcpServer m_tcpServer_2;
    QTimer *m_sendTimer;
    QTimer *m_testTimer;
    AgreementZXJC m_agreementZXJC;
    QStringList tcpData_1;
    QStringList tcpData_1_2;

    void on_sendTimer_out();
    void on_testTimer_out();
    void handleUiSetting();   //处理界面上的设置项
    void initTcpData_1();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
