//*********************************************************************************************
//* Programme : ClientTCPIP.cpp                                      Date : 22/11/2024
//*--------------------------------------------------------------------------------------------
//* Derni�re mise � jour : 05/12/2024
//*
//* Programmeurs : Fauquembergue Yann                                    Classe : BTSCIEL2
//*                Quadrao Gabin
//*--------------------------------------------------------------------------------------------
//* But : Interface de gestion de client TCP/IP qui envoie des requ�tes de temp�rature et
//*       re�oit, si possible, les valeurs demand�es
//* 
//*********************************************************************************************

#include "ClientTCPIP.h"

//---------------------------------------------------------------------------------------------
//* Constructeur de la classe `ClientTCPIP`.
//* Initialise les variables et configure l'IHM
//* Param�tres :
//*  - QWidget* parent : widget parent (nullptr par d�faut pour une fen�tre principale).
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
ClientTCPIP::ClientTCPIP(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    socket = new QTcpSocket(this);
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(OnSocketConnected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(OnSocketDisconnected()));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(OnSocketReadyRead()));

}

//---------------------------------------------------------------------------------------------
//* Fonction `SendRequest`.
//* M�thode qui envoie une requ�te au serveur
//* Param�tres : QString &request : La requ�te � envoyer.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::SendRequest(QString &request)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QTextStream stream(socket);
        stream << request;
        socket->flush();
    }
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnConnectButtonClicked`.
//* M�thode slot appel�e lorsque le bouton de connexion est appuy�.
//* Param�tres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnConnectButtonClicked()
{
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
        ui.errorLabel->setText("ERREUR: Port invalide");
        ui.connectStatus->setText("Etat connexion : Deconnecte");
    }
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnSocketConnected`.
//* M�thode slot appel�e lorsque le socket est connect�
//* Param�tres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketConnected()
{
    ui.connectStatus->setText("Etat connexion : Connecte");
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnSocketDisconnected`.
//* M�thode slot appel�e lorsque le socket est d�connect�
//* Param�tres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketDisconnected()
{
    ui.connectStatus->setText("Etat connexion : Deconnecte");
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnSocketReadyRead`.
//* M�thode slot appel�e lorsque le socket re�oit des donn�es
//* Param�tres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketReadyRead()
{
    QByteArray data = socket->readAll();
    QString response(data);

    QString formattedResponse;

    // R�ponse de type temp�rature Celsius
    if (response.startsWith("Td")) {
        QString sensorId = response.mid(2, 2);
        QString temperature = response.mid(5);
        formattedResponse = QString("La temperature en Celsius du capteur %1 est de %2.C").arg(sensorId, temperature);
    }
    // R�ponse de type temp�rature Fahrenheit
    else if (response.startsWith("Tf")) {
        QString sensorId = response.mid(2, 2);
        QString temperature = response.mid(5);
        formattedResponse = QString("La temperature en Fahrenheit du capteur %1 est de %2.F").arg(sensorId, temperature);
    }
    // R�ponse de type hygrom�trie
    else if (response.startsWith("Hr")) {
        QString sensorId = response.mid(2, 2);
        QString humidity = response.mid(5);
        formattedResponse = QString("L'hygrometrie du capteur %1 est de %2%").arg(sensorId, humidity);
    }
    // R�ponse inconnue
    else {
        formattedResponse = "R�ponse inconnue re�ue du serveur: " + response;
    }

    ui.dataBox->addItem(formattedResponse);
    ui.dataBox->scrollToBottom();
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnGetTempButtonClicked`.
//* M�thode slot appel�e lorsque le bouton de requ�te de temp�rature est appuy�. Elle tire les
//      informations sur l'IHM et envoie une requ�te
//* Param�tres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnGetTempButtonClicked()
{
    if (ui.tempCombo->currentIndex() < 0) { return; }

    QString data;
    QString sensorId = ui.sensorLine->text().toStdString().c_str();
    switch (ui.tempCombo->currentIndex())
    {
        case 0:
            data += "Td" + sensorId + "?";
            SendRequest(data);
            break;
        case 1:
            data += "Tf" + sensorId + "?";
            SendRequest(data);
            break;
        case 2:
            data += "Hr" + sensorId + "?";
            SendRequest(data);
            break;

        default:
            break;
    }
}
