//*********************************************************************************************
//* Programme : ClientTCPIP.cpp                                      Date : 22/11/2024
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 05/12/2024
//*
//* Programmeurs : Fauquembergue Yann                                    Classe : BTSCIEL2
//*                Quadrao Gabin
//*--------------------------------------------------------------------------------------------
//* But : Interface de gestion de client TCP/IP qui envoie des requêtes de température et
//*       reçoit, si possible, les valeurs demandées
//* 
//*********************************************************************************************

#include "ClientTCPIP.h"

//---------------------------------------------------------------------------------------------
//* Constructeur de la classe `ClientTCPIP`.
//* Initialise les variables et configure l'IHM
//* Paramètres :
//*  - QWidget* parent : widget parent (nullptr par défaut pour une fenêtre principale).
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
//* Méthode qui envoie une requête au serveur
//* Paramètres : QString &request : La requête à envoyer.
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
//* Méthode slot appelée lorsque le bouton de connexion est appuyé.
//* Paramètres : aucun.
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
//* Méthode slot appelée lorsque le socket est connecté
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketConnected()
{
    ui.connectStatus->setText("Etat connexion : Connecte");
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnSocketDisconnected`.
//* Méthode slot appelée lorsque le socket est déconnecté
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketDisconnected()
{
    ui.connectStatus->setText("Etat connexion : Deconnecte");
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnSocketReadyRead`.
//* Méthode slot appelée lorsque le socket reçoit des données
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ClientTCPIP::OnSocketReadyRead()
{
    QByteArray data = socket->readAll();
    QString response(data);

    QString formattedResponse;

    // Réponse de type température Celsius
    if (response.startsWith("Td")) {
        QString sensorId = response.mid(2, 2);
        QString temperature = response.mid(5);
        formattedResponse = QString("La temperature en Celsius du capteur %1 est de %2.C").arg(sensorId, temperature);
    }
    // Réponse de type température Fahrenheit
    else if (response.startsWith("Tf")) {
        QString sensorId = response.mid(2, 2);
        QString temperature = response.mid(5);
        formattedResponse = QString("La temperature en Fahrenheit du capteur %1 est de %2.F").arg(sensorId, temperature);
    }
    // Réponse de type hygrométrie
    else if (response.startsWith("Hr")) {
        QString sensorId = response.mid(2, 2);
        QString humidity = response.mid(5);
        formattedResponse = QString("L'hygrometrie du capteur %1 est de %2%").arg(sensorId, humidity);
    }
    // Réponse inconnue
    else {
        formattedResponse = "Réponse inconnue reçue du serveur: " + response;
    }

    ui.dataBox->addItem(formattedResponse);
    ui.dataBox->scrollToBottom();
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnGetTempButtonClicked`.
//* Méthode slot appelée lorsque le bouton de requête de température est appuyé. Elle tire les
//      informations sur l'IHM et envoie une requête
//* Paramètres : aucun.
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
