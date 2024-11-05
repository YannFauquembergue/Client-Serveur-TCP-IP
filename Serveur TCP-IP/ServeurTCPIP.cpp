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
}

void ServeurTCPIP::OnLogClearButtonClicked()
{
    ui.logBox->clear();
}
