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
    QString str(data);
    obj->write(data);
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
