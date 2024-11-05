#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientTCPIP.h"
#include <qtcpsocket.h>

class ClientTCPIP : public QMainWindow
{
    Q_OBJECT

public:
    ClientTCPIP(QWidget *parent = nullptr);

private:
    Ui::ClientTCPIPClass ui;
    QTcpSocket* socket;

public slots:
    void OnConnectButtonClicked();

    void OnSocketConnected();
    void OnSocketDisconnected();
    void OnSocketReadyRead();
};
