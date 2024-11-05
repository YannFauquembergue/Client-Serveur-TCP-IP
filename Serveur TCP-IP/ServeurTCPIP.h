#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ServeurTCPIP.h"
#include <qtcpserver.h>

class ServeurTCPIP : public QMainWindow
{
    Q_OBJECT

public:
    ServeurTCPIP(QWidget *parent = nullptr);

private:
    Ui::ServeurTCPIPClass ui;
    QTcpServer* server;

public slots:
    void OnListenButtonClicked();
    void OnServerNewConnection();

    void OnLogClearButtonClicked();
};
