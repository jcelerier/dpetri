#include <QtNetwork>
#include <QDebug>
#include <zeroconf/bonjourserviceregister.h>
#include "server.h"
#include "mainwindow.h"

ZeroconfServer::ZeroconfServer(MainWindow *parent):
	QObject(parent)
{
	tcpServer = new QTcpServer(this);

	if (!tcpServer->listen())
	{
		qDebug() << "Unable to start the server: "
				 << tcpServer->errorString();
		return;
	}

	connect(tcpServer, SIGNAL(newConnection()),
			this,	   SLOT(sendConnectionData()));

	bonjourRegister = new BonjourServiceRegister(this);
	bonjourRegister->registerService(BonjourRecord(tr("Distributed Petri Net on %1").arg(QHostInfo::localHostName()),
												   QLatin1String("_dpetriserver._tcp"),
												   QString()),
									 tcpServer->serverPort());
}

void ZeroconfServer::sendConnectionData()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_5_2);
	out << (quint16) 0;
	out << tcpServer->serverAddress()
		<< (quint16) dynamic_cast<MainWindow*>(parent())->logic.localClient.port();
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));

	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

	connect(clientConnection, SIGNAL(disconnected()),
			clientConnection, SLOT(deleteLater()));

	clientConnection->write(block);
	clientConnection->disconnectFromHost();
}
