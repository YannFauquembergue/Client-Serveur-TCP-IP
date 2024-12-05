//*********************************************************************************************
//* Programme : ServeurTCPIP.cpp                                      Date : 22/11/2024
//*--------------------------------------------------------------------------------------------
//* Dernière mise à jour : 05/12/2024
//*
//* Programmeurs : Fauquembergue Yann                                    Classe : BTSCIEL2
//*                Quadrao Gabin
//*--------------------------------------------------------------------------------------------
//* But : Interface de gestion de serveur TCP/IP qui traite des données de température
//*       
//* 
//*********************************************************************************************

#include "ServeurTCPIP.h"
#include <qnetworkinterface.h>

//---------------------------------------------------------------------------------------------
//* Constructeur de la classe `ServeurTCPIP`.
//* Initialise les variables et configure l'IHM
//* Paramètres :
//*  - QWidget* parent : widget parent (nullptr par défaut pour une fenêtre principale).
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
ServeurTCPIP::ServeurTCPIP(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    server = new QTcpServer(this);
    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(OnServerNewConnection()));
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnListenButtonClicked`.
//* Méthode slot appelée lorsque le bouton d'écoute de port est appuyé.
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ServeurTCPIP::OnListenButtonClicked()
{
    if (!server->isListening()) {
        QString port = ui.portLine->text();

        bool ok;
        int portAsInt = port.toInt(&ok);
        if (ok)
        {
            if (server->listen(QHostAddress::AnyIPv4, portAsInt));
            {
                ui.listenStatus->setText("Etat serveur : En ecoute sur le port " + port);
                ui.ipLabel->setText(QString("IP: %1").arg(getLocalIPv4Address()));
                ui.listenButton->setText("Fermer");
            }
        }
        else
        {
            ui.listenStatus->setText("Etat serveur : HS [ERREUR: Port invalide]");
        }
    }
    else
    {
        server->close();
        ui.listenStatus->setText("Etat serveur : HS");
        ui.ipLabel->setText("IP: -");
        ui.listenButton->setText("Mettre en ecoute");
    }
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnServerNewConnection`.
//* Méthode slot appelée lorsque le serveur reçoit une nouvelle connexion
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ServeurTCPIP::OnServerNewConnection()
{
    ui.logBox->addItem("Nouvelle connexion client !");
    ui.logBox->scrollToBottom();
    QTcpSocket* client = server->nextPendingConnection();
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(OnClientReadyRead()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(OnClientDisconnected()));
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnClientReadyRead`.
//* Méthode slot appelée lorsque le serveur reçoit des données d'un client.
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ServeurTCPIP::OnClientReadyRead()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = obj->read(obj->bytesAvailable());
    QString request(data);

    ui.logBox->addItem("Donnees recues: " + request);
    ui.logBox->scrollToBottom();

    QString response;

    // Requête est de type température Celsius ?
    if (request.startsWith("Td") && request.endsWith("?")) {
        QString sensorId = request.mid(2, request.length() - 3);
        response = RespondWithTemperature("Td", sensorId);
    }
    // Requête est de type température Fahrenheit ?
    else if (request.startsWith("Tf") && request.endsWith("?")) {
        QString sensorId = request.mid(2, request.length() - 3);
        response = RespondWithTemperature("Tf", sensorId);
    }
    // Requête est de hygrométrie ?
    else if (request.startsWith("Hr") && request.endsWith("?")) {
        QString sensorId = request.mid(2, request.length() - 3);
        response = RespondWithHumidity(sensorId);
    }
    // Requête inconnue si aucun des trois types ci-dessus est reconnu
    else {
        response = "ERREUR: Requete inconnue";
    }

    obj->write(response.toStdString().c_str());
    ui.logBox->addItem("Donnees envoyees: " + response);
    ui.logBox->scrollToBottom();
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnClientDisconnected`.
//* Méthode slot appelée lorsqu'un client se déconnecte du serveur.
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ServeurTCPIP::OnClientDisconnected()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender());
    QObject::disconnect(obj, SIGNAL(readyRead()), this, SLOT(OnClientReadyRead));
    QObject::disconnect(obj, SIGNAL(disconnected()), this, SLOT(OnClientDisconnected));
    obj->deleteLater();
}

//---------------------------------------------------------------------------------------------
//* Fonction `OnLogClearButtonClicked`.
//* Méthode slot appelée lorsque le bouton pour effacer le log est appuyé.
//* Paramètres : aucun.
//* Valeur de retour : aucune.
//---------------------------------------------------------------------------------------------
void ServeurTCPIP::OnLogClearButtonClicked()
{
    ui.logBox->clear();
}

//---------------------------------------------------------------------------------------------
//* Fonction `RespondWithTemperature`.
//* Génère une réponse de type température en Celsius ou Fahrenheit.
//* Paramètres :
//      - const QString& requestType : Le type de température (°C ou °F ?)
//      - QString& sensorId : L'id du capteur
//* Valeur de retour : Message de type "[Type][Capteur],[Température]"
//---------------------------------------------------------------------------------------------
QString ServeurTCPIP::RespondWithTemperature(const QString& requestType, QString& sensorId)
{
    double temperature = GenerateRandom(-20, 37);
    char sign = temperature >= 0 ? '+' : '-'; // On cherche à rendre visible le "+" si la valeur est positive
    QString tempType = requestType == "Td" ? "Td" : "Tf";

    if (requestType == "Tf") {
        temperature = temperature * 9/5 + 32;
    }

    return QString("%1%2,%3%4").arg(tempType).arg(sensorId, 2, '0').arg(sign).arg(qAbs(temperature), 0, 'f', 2);
}

//---------------------------------------------------------------------------------------------
//* Fonction `RespondWithHumidity`.
//* Génère une réponse de type hygrométrie.
//* Paramètres :
//      - QString& sensorId : L'id du capteur
//* Valeur de retour : Message de type "Hr[Capteur],[Hygrométrie]"
//---------------------------------------------------------------------------------------------
QString ServeurTCPIP::RespondWithHumidity(QString& sensorId)
{
    double humidity = GenerateRandom(0, 99.9);
    return QString("Hr%1,%2").arg(sensorId, 2, '0').arg(humidity, 0, 'f', 1);
}

double ServeurTCPIP::GenerateRandom(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

//---------------------------------------------------------------------------------------------
//* Fonction `getLocalIPv4Address`.
//* Obtient la première adresse IPv4 locale disponible.
//* Paramètres : aucun.
//* Valeur de retour : Adresse IPv4 locale si possible, sinon 0.0.0.0
//---------------------------------------------------------------------------------------------
QString ServeurTCPIP::getLocalIPv4Address() {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            for (const QNetworkAddressEntry& entry : interface.addressEntries()) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    return entry.ip().toString();
                }
            }
        }
    }
    return "0.0.0.0";
}

