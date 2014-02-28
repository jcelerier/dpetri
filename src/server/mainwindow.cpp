#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>
#include <sstream>
#include <osctools.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new Server(this)),
	pnmodel(this)
{
	ui->setupUi(this);
	ui->centralwidget->setPetriNetModel(pnmodel);
	ui->centralwidget->setOscManager(clientMgr);

	connect(ui->actionLoad_a_Petri_Net, SIGNAL(triggered()),
			&pnmodel,					SLOT(loadFile()));

	connect(&pnmodel,		   SIGNAL(poolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	connect(this,			   SIGNAL(connectionListChanged()),
			ui->centralwidget, SLOT(updateConnectionList()));

	connect(this,			   SIGNAL(localPoolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	connect(this,			   SIGNAL(clientPoolChanged(int)),
			ui->centralwidget, SLOT(updateClientPool(int)));

	connect(ui->centralwidget, SIGNAL(play()),
			&pnmodel,		   SLOT(start()));

	receiver.addHandler("/connect/discover",
						std::bind(&MainWindow::handleConnection, this, std::placeholders::_1));

	receiver.addHandler("/pool/take",
						std::bind(&MainWindow::handleTake, this, std::placeholders::_1));

	receiver.addHandler("/pool/give",
						std::bind(&MainWindow::handleGive, this, std::placeholders::_1));

	receiver.run();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::handleConnection(osc::ReceivedMessageArgumentStream args)
{
	//// Récupération des données extérieures
	int id; // inutilisé
	const char* hostname;
	const char* ip;
	osc::int32 port;

	args >> id >> hostname >> ip >> port >> osc::EndMessage;

	//// Création de l'émetteur vers le client
	auto& newClient = clientMgr.createConnection(std::string(hostname),
												 std::string(ip),
												 port);

	newClient.send("/connect/set_id",
				   newClient.id());

	emit connectionListChanged();

	//// Envoi du réseau de petri vers le nouveau client
	pnmodel.dumpTo(newClient);

	//// Envoi du pool vers le nouveau client
	pnmodel.pool().dumpTo(0, newClient);

	//// Envoi des informations des autres clients vers le nouveau client
	for(RemoteClient& c : clientMgr)
		newClient.initConnectionTo(c);
}

void MainWindow::handleTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 idRemote;
	osc::int32 idNode;

	args >> idRemote >> idNode >> osc::EndMessage;

	auto& client = clientMgr[idRemote];

	// Vérifier si la node est bien dans le pool
	client.take(idNode, pnmodel.pool());
	client.send("/pool/ackTake",
				0,
				(osc::int32) idNode); // Cas serveur

	//Mise-à-jour de l'image du pool serveur des autres clients
	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != idRemote)
			pnmodel.pool().dumpTo(0, c);
	}

	emit localPoolChanged();
	emit clientPoolChanged(client.id());
}

void MainWindow::handleGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 idRemote;
	osc::int32 idNode;

	args >> idRemote >> idNode >> osc::EndMessage;

	auto& client = clientMgr[idRemote];

	// Vérifier si la node est bien dans le pool
	client.give(idNode, pnmodel.pool());
	client.send("/pool/ackGive",
				0,
				(osc::int32) idNode);

	//Mise-à-jour des pools des autres clients
	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != idRemote)
			pnmodel.pool().dumpTo(0, c);
	}

	emit localPoolChanged();
	emit clientPoolChanged(client.id());
}

