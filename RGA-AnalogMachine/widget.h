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

private:
    TcpServer m_tcpServer;
    TcpServer m_tcpServer_2;
    QTimer *m_sendTimer;
    AgreementZXJC m_agreementZXJC;
    QStringList tcpData_1;

    void on_sendTimer_out();
    void handleUiSetting();   //处理界面上的设置项
    void initTcpData_1();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
