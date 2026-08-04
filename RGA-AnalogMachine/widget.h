#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "TcpServer.h"
#include <QTimer>

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
    TcpServer m_tcpServer_2;
    QTimer *m_sendTimer;

    void on_sendTimer_out();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
