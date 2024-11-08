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

void ClientTCPIP::SendRequest(QString &request)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QTextStream stream(socket);
        stream << request;
        socket->flush();
    }
}

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
    QByteArray data = socket->readAll();
    QString str(data);

    // Affiche la réponse dans dataBox
    ui.dataBox->insertPlainText(str + "\n");
    ui.dataBox->ensureCursorVisible();
}

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
            data += "H" + sensorId + "?";
            SendRequest(data);
            break;

        default:
            break;
    }
}
