#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ServeurTCPIP.h"
#include <qtcpsocket.h>
#include <qtcpserver.h>
#include <QtGlobal>

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
    void OnClientReadyRead();
    void OnClientDisconnected();

    void OnLogClearButtonClicked();

    QString RespondWithTemperature(const QString& requestType, const QString& sensorId);
    QString RespondWithHumidity(const QString& sensorId);
};
