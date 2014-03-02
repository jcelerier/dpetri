#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>
#include <QCoreApplication>
#include <sstream>
#include <osctools.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new ZeroconfServer(this)),
	localClient(0, // Est assigné par serveur
				"server",
				"0.0.0.0",
				7000,
				std::bind(&MainWindow::localNetChanged, this),
				std::bind(&MainWindow::localPoolChanged, this))
{
	ui->setupUi(this);
	ui->centralwidget->setModel(localClient);
	ui->centralwidget->setOscManager(clientMgr);

	connect(ui->actionLoad_a_Petri_Net, SIGNAL(triggered()),
			this,					SLOT(loadNetAndPoolFromFile()));

	connect(this,			   SIGNAL(connectionListChanged()),
			ui->centralwidget, SLOT(updateConnectionList()));

	connect(this,			   SIGNAL(localPoolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	connect(this,			   SIGNAL(clientPoolChanged(int)),
			ui->centralwidget, SLOT(updateClientPool(int)));

	connect(this, SIGNAL(localNetChanged()),
			ui->centralwidget, SLOT(updateNet()));

	localClient.receiver().addHandler("/connect/discover",
									  &MainWindow::handleConnection, this);

	localClient.receiver().addHandler("/pool/take",
									  &MainWindow::handleTake, this);

	localClient.receiver().addHandler("/pool/give",
									  &MainWindow::handleGive, this);

/*	receiver.addHandler("/petrinet/addToken",
						std::bind(&PetriNetModel::handleAddToken,
								  pnmodel,
								  std::placeholders::_1));

	receiver.addHandler("/petrinet/removeToken",
						std::bind(&PetriNetModel::handleRemoveToken,
								  pnmodel,
								  std::placeholders::_1));
*/

	auto x = QCoreApplication::arguments();
	if(x.size() > 1)
	{
		loadFromFile(x.at(1));
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadNetAndPoolFromFile()
{
	QString file = QFileDialog::getOpenFileName(nullptr,
												tr("Choose a Petri Net"),
												QString(),
												tr("FIONA File (*.fiona)"));
	loadFromFile(file);
}

void MainWindow::handleConnection(osc::ReceivedMessageArgumentStream args)
{
	//// Récupération des données extérieures
	int id; // inutilisé
	const char* hostname;
	const char* ip;
	osc::int32 port;

	args  >> hostname >> id >> ip >> port >> osc::EndMessage;

	//// Création de l'émetteur vers le client
	auto& newClient = clientMgr.createConnection(std::string(hostname),
												 std::string(ip),
												 port);

	newClient.send("/connect/set_id",
				   newClient.id());

	emit connectionListChanged();

	//// Envoi du réseau de petri vers le nouveau client
	localClient.model().dumpTo(newClient);

	//// Envoi du pool vers le nouveau client
	localClient.pool().dumpTo(0, newClient);

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
	client.take(idNode, localClient.pool());
	client.send("/pool/ackTake",
				0,
				(osc::int32) idNode); // Cas serveur

	//Mise-à-jour de l'image du pool serveur des autres clients
	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != idRemote)
			localClient.pool().dumpTo(0, c);
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
	client.give(idNode, localClient.pool());
	client.send("/pool/ackGive",
				0,
				(osc::int32) idNode);

	//Mise-à-jour des pools des autres clients
	for(RemoteClient& c : clientMgr)
	{
		if(c.id() != idRemote)
			localClient.pool().dumpTo(0, c);
	}

	emit localPoolChanged();
	emit clientPoolChanged(client.id());
}

void MainWindow::loadFromFile(QString s)
{
	std::ifstream f(s.toStdString());
	localClient.loadNetAndPool(f);
}

