#include "ServeurTCPIP.h"

ServeurTCPIP::ServeurTCPIP(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    server = new QTcpServer(this);
    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(OnServerNewConnection()));
}

void ServeurTCPIP::OnListenButtonClicked()
{
    if (!server->isListening()) {
        QString port = ui.portLine->text();

        bool ok;
        int portAsInt = port.toInt(&ok);
        if (ok)
        {
            server->listen(QHostAddress::AnyIPv4, portAsInt);
            ui.listenStatus->setText("Etat serveur : En ecoute sur le port " + port);
            ui.listenButton->setText("Fermer");
        }
    }
    else
    {
        server->close();
        ui.listenStatus->setText("Etat serveur : HS");
        ui.listenButton->setText("Mettre en ecoute");
    }
}

void ServeurTCPIP::OnServerNewConnection()
{
    ui.logBox->insertPlainText("Nouvelle connexion client !\n");
    QTcpSocket* client = server->nextPendingConnection();
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(OnClientReadyRead()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(OnClientDisconnected()));
}

void ServeurTCPIP::OnClientReadyRead()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = obj->read(obj->bytesAvailable());
    QString request(data);

    QString response;

    // Requ�te est de type temp�rature Celsius ?
    if (request.startsWith("Td") && request.endsWith("?")) {
        QString sensorId = request.mid(2, request.length() - 3);
        response = RespondWithTemperature("Td", sensorId);
    }
    // Requ�te est de type temp�rature Fahrenheit ?
    else if (request.startsWith("Tf") && request.endsWith("?")) {
        QString sensorId = request.mid(2, request.length() - 3);
        response = RespondWithTemperature("Tf", sensorId);
    }
    // Requ�te est de hygrom�trie ?
    else if (request.startsWith("H") && request.endsWith("?")) {
        QString sensorId = request.mid(1, request.length() - 2);
        response = RespondWithHumidity(sensorId);
    }
    // Requ�te inconnue si aucun des trois types ci-dessus est reconnu
    else {
        response = "ERREUR: Requ�te inconnue";
    }

    obj->write(response.toUtf8());
}

void ServeurTCPIP::OnClientDisconnected()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender());
    QObject::disconnect(obj, SIGNAL(readyRead()), this, SLOT(OnClientReadyRead));
    QObject::disconnect(obj, SIGNAL(disconnected()), this, SLOT(OnClientDisconnected));
    obj->deleteLater();
}

void ServeurTCPIP::OnLogClearButtonClicked()
{
    ui.logBox->clear();
}

// R�ponse temp�rature (Celsius / Fahrenheit)
QString ServeurTCPIP::RespondWithTemperature(const QString& requestType, const QString& sensorId)
{
    double temperature = (qrand() % 5700 - 2000) / 100.0;
    char sign = temperature >= 0 ? '+' : '-';
    QString tempType = requestType == "Td" ? "Td" : "Tf";

    if (requestType == "Tf") {
        temperature = temperature * 9.0 / 5.0 + 32.0;
    }

    return QString("%1%2,%3%4").arg(tempType).arg(sensorId, 2, '0').arg(sign).arg(qAbs(temperature), 0, 'f', 2);
}

// R�ponse hygrom�trie
QString ServeurTCPIP::RespondWithHumidity(const QString& sensorId)
{
    double humidity = (qrand() % 1000) / 10.0;
    return QString("Hr%1,%2").arg(sensorId, 2, '0').arg(humidity, 0, 'f', 1);
}

