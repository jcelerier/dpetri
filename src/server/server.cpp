#include <QtNetwork>
#include <QDebug>
#include <zeroconf/bonjourserviceregister.h>
#include "server.h"
#include "mainwindow.h"

ZeroconfServer::ZeroconfServer(MainWindow *parent):
	QObject(parent)
{
	tcpServer = new QTcpServer(this);

	if (!tcpServer->listen(QHostAddress::Any, 42591))
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

	qDebug() << "I am listening on port : " << tcpServer->serverPort();
}

void ZeroconfServer::sendConnectionData()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

	out.setVersion(QDataStream::Qt_5_2);
	out << (quint16) 0;
	out << clientConnection->localAddress()
		<< (quint16) dynamic_cast<MainWindow*>(parent())->logic.localClient.port();
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));

	connect(clientConnection, SIGNAL(disconnected()),
			clientConnection, SLOT(deleteLater()));

	clientConnection->write(block);
	clientConnection->disconnectFromHost();

	qDebug() << "Data sent";
}
