#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <array>
#include <functional>
#include <sstream>
#include <QtNetwork/QNetworkInterface>
#include <osc/OscOutboundPacketStream.h>
#include <osctools.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	connectDialog(new ZeroconfConnectDialog(this)),
	pnmodel(this),
	clientMgr(pnmodel.pool)
{
	ui->setupUi(this);

	ui->centralwidget->setPetriNetModel(pnmodel);
	ui->centralwidget->setParent(this);

	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newConnection(QHostAddress,quint16)));

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

	receiver.addHandler("/connect/id",
						std::bind(&MainWindow::handleIdReception,
								  this,
								  std::placeholders::_1));

	receiver.addHandler("/pool/dump",
						std::bind(&MainWindow::handleDump,
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

	receiver.run();
}

MainWindow::~MainWindow()
{
	delete ui;
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

	localId = id;
	qDebug() << "I got id " << id << " !";
}

void MainWindow::handleAckTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = clientMgr[id];
	origin.give(nodeId, pnmodel.pool);

	emit poolChanged();
	emit localPoolChanged();
}


void MainWindow::handleAckGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = clientMgr[id];
	origin.take(nodeId, pnmodel.pool);

	emit poolChanged();
	emit localPoolChanged();
}


void MainWindow::handleDump(osc::ReceivedMessageArgumentStream args)
{
	osc::Blob b;
	args >> b >> osc::EndMessage;

	// Charger dans le pool du client 0
	auto& server = clientMgr[0];

	server.pool().load(pnmodel.net, static_cast<const char*>(b.data));
	emit poolChanged();

}

void MainWindow::openConnectionDialog()
{
	connectDialog->exec();
}

QString getIp(QHostAddress serverIP)
{
	// Cas local
	QString local{"127.0.0.1"};
	if(serverIP == QHostAddress::LocalHost) return local;

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

void MainWindow::newConnection(QHostAddress ip, quint16 port)
{
	if(ip.toString() == QString("0.0.0.0"))
		ip = QHostAddress::LocalHost;

	auto& sender = clientMgr.createConnection("server", ip.toString().toStdString(), port);

	osc::MessageGenerator m;
	sender.send(m("/connect",
				  QHostInfo::localHostName().toStdString().c_str(),
				  getIp(ip).toStdString().c_str(),
				  (osc::int32) receiver.port()));
}

void MainWindow::takeNode(QString s)
{
	// Faire un take vers le pool du client d'id 0 (le serveur)
	auto& server = clientMgr[0];
	auto& node = server.pool()[s.toStdString()];

	server.send(osc::MessageGenerator()
				("/pool/take", (osc::int32) localId, (osc::int32) node.id));
}

void MainWindow::giveNode(QString s)
{
	// Faire un give vers le pool du client d'id 0 (le serveur)
	auto& server = clientMgr[0];
	auto& node = pnmodel.pool[s.toStdString()];

	server.send(osc::MessageGenerator()
				("/pool/give", (osc::int32) localId, (osc::int32) node.id));
}
