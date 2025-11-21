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

private:
    TcpServer m_tcpServer;
    QTimer *m_sendTimer;
    AgreementZXJC m_agreementZXJC;

    void on_sendTimer_out();
    void handleUiSetting();   //处理界面上的设置项

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
