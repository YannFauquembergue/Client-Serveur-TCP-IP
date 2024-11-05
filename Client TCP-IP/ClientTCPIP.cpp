#include "ClientTCPIP.h"

ClientTCPIP::ClientTCPIP(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    socket = new QTcpSocket(this);
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(OnSocketConnected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(OnSocketDisconnected()));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(OnSocketReadyRead()));

}

void ClientTCPIP::OnConnectButtonClicked()
{
    ui.connectStatus->setText("Etat connexion : Connexion...");
    QString ip = ui.ipLine->text();
    QString port = ui.portLine->text();

    bool ok;
    int portAsInt = port.toInt(&ok);
    if (ok)
    {
        ui.errorLabel->setText("");
        socket->connectToHost(ip, portAsInt);
    }
    else
    {
        ui.errorLabel->setText("ERREUR: Echec de la connexion.");
        ui.connectStatus->setText("Etat connexion : Deconnecte");
    }
}

void ClientTCPIP::OnSocketConnected()
{
    ui.connectStatus->setText("Etat connexion : Connecte");
}

void ClientTCPIP::OnSocketDisconnected()
{
    ui.connectStatus->setText("Etat connexion : Deconnecte");
}

void ClientTCPIP::OnSocketReadyRead()
{
    ui.dataBox->insertPlainText("placeholder");
}