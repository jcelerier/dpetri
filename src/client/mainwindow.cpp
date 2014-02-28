#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <array>
#include <functional>
#include <sstream>
#include <QtNetwork/QNetworkInterface>
#include <osc/OscOutboundPacketStream.h>
#include <osctools.h>


QString getIp(QHostAddress serverIP)
{
	// Cas local
	if(serverIP == QHostAddress::LocalHost) return "127.0.0.1";

	// Cas distant
	QList<QHostAddress> list = QNetworkInterface::allAddresses();

	for(int nIter=0; nIter<list.count(); nIter++)
	{
		if(!list[nIter].isLoopback())
			if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
				return list[nIter].toString();
	}

	return "0.0.0.0";
}



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	connectDialog(new ZeroconfConnectDialog(this)),
	pnmodel(this),
	localClient(-1, // Est assigné par serveur
				QHostInfo::localHostName().toStdString(),
				getIp(QHostAddress::LocalHost).toStdString(),
				receiver.port())
{
	ui->setupUi(this);

	ui->centralwidget->setPetriNetModel(pnmodel);
	ui->centralwidget->setParent(this);

	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newServerConnection(QHostAddress,quint16)));

	connect(&pnmodel,		   SIGNAL(poolChanged()),
			ui->centralwidget, SLOT(updatePool()));

	connect(ui->centralwidget, SIGNAL(take(QString)),
			this,			   SLOT(takeNode(QString)));

	connect(ui->centralwidget, SIGNAL(give(QString)),
			this,			   SLOT(giveNode(QString)));

	connect(this,			   SIGNAL(poolChanged()),
			ui->centralwidget, SLOT(updatePool()));

	connect(this,			   SIGNAL(localPoolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	receiver.addHandler("/petrinet/dump",
						std::bind(&MainWindow::handlePetriNetReception,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/connect/set_id",
						std::bind(&MainWindow::handleIdReception,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/pool/dump",
						std::bind(&MainWindow::handlePoolDump,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/pool/ackTake",
						std::bind(&MainWindow::handleAckTake,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/pool/ackGive",
						std::bind(&MainWindow::handleAckGive,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/pool/ackGive",
						std::bind(&MainWindow::handleConnectDiscover,
								  this,
								  std::placeholders::_1));

	receiver.run();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::handleConnectDiscover(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	const char* name;
	const char* ip;
	osc::int32 port;

	args >> name >> id >> ip >> port >> osc::EndMessage;

	if(clientMgr.hasClient(id)) return;

	auto& remote = clientMgr.createConnection(id,
											  std::string(name),
											  std::string(ip),
											  port);

	//// Lui faire la demande de connection
	remote.initConnectionTo(localClient);

	//TODO attention si le réseau lag ici ? faire une attente ?
	//// Lui envoyer notre pool
	pnmodel.pool().dumpTo(localClient.id(), remote);
}

void MainWindow::handlePetriNetReception(osc::ReceivedMessageArgumentStream args)
{
	osc::Blob b;
	args >> b >> osc::EndMessage;

	pnmodel.loadFromString(std::string(static_cast<const char*>(b.data)));
}

void MainWindow::handleIdReception(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	args >> id >> osc::EndMessage;

	localClient.setId(id);
	qDebug() << "I got id " << id << " !";
}

void MainWindow::handleAckTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = clientMgr[id];
	origin.give(nodeId, pnmodel.pool());

	emit poolChanged();
	emit localPoolChanged();

	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != 0) // Pas le serveur (why not ?)
			pnmodel.pool().dumpTo(localClient.id(), c);
	}

	// Potentiellement unifier en rajoutant un message
	// "/pool/confirm id-from id-to id-node"
	// que chacun applique ?
	// -> plus besoin d'envoyer les pool (mais cas ou ils ne communiquent pas ?)
}


void MainWindow::handleAckGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = clientMgr[id];
	origin.take(nodeId, pnmodel.pool());

	emit poolChanged();
	emit localPoolChanged();

	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != 0) // Pas le serveur (why not?)
			pnmodel.pool().dumpTo(localClient.id(), c);
	}
}

void MainWindow::handlePoolDump(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::Blob b;
	args >> id >> b >> osc::EndMessage;

	// Charger dans le pool du client 0
	clientMgr[id].pool().load(pnmodel.net(), static_cast<const char*>(b.data));

	if(id == 0) emit poolChanged(); // Serveur
}

void MainWindow::openConnectionDialog()
{
	connectDialog->exec();
}

void MainWindow::newServerConnection(QHostAddress ip, quint16 port)
{
	// Connection via zeroconf
	if(ip.toString() == QString("0.0.0.0"))
		ip = QHostAddress::LocalHost;

	auto& sender = clientMgr.createConnection("server", ip.toString().toStdString(), port);

	sender.initConnectionTo(localClient);
}

void MainWindow::takeNode(QString s)
{
	// Faire un take vers le pool du client d'id 0 (le serveur)
	auto& server = clientMgr[0];
	auto& node = server.pool()[s.toStdString()];

	server.send("/pool/take",
				(osc::int32) localClient.id(),
				(osc::int32) node.id);
}

void MainWindow::giveNode(QString s)
{
	// Faire un give vers le pool du client d'id 0 (le serveur)
	auto& server = clientMgr[0];
	auto& node = pnmodel.pool()[s.toStdString()];

	server.send("/pool/give",
				(osc::int32) localClient.id(),
				(osc::int32) node.id);
}
