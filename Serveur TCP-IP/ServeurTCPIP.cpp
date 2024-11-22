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
    ui.logBox->addItem("Nouvelle connexion client !");
    ui.logBox->scrollToBottom();
    QTcpSocket* client = server->nextPendingConnection();
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(OnClientReadyRead()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(OnClientDisconnected()));
}

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
        response = "ERREUR: Requête inconnue";
    }

    obj->write(response.toStdString().c_str());
    ui.logBox->addItem("Donnees envoyees: " + response);
    ui.logBox->scrollToBottom();
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

// Réponse température (Celsius / Fahrenheit)
QString ServeurTCPIP::RespondWithTemperature(const QString& requestType, QString& sensorId)
{
    double temperature = GenerateRandom(-20, 37);
    char sign = temperature >= 0 ? '+' : '-';
    QString tempType = requestType == "Td" ? "Td" : "Tf";

    if (requestType == "Tf") {
        temperature = temperature * 9/5 + 32;
    }

    return QString("%1%2,%3%4").arg(tempType).arg(sensorId, 2, '0').arg(sign).arg(qAbs(temperature), 0, 'f', 2);
}

// Réponse hygrométrie
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

