#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>
#include <sstream>
#include <osctools.h>
#include <petrinettools.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new Server(this)),
	pnmodel(this),
	manager(pnmodel.pool)
{
	ui->setupUi(this);
	ui->centralwidget->setPetriNetModel(pnmodel);
	ui->centralwidget->setOscManager(manager);

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

	receiver.addHandler("/connect",
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
	const char* hostname;
	const char* ip;
	osc::int32 port;

	args >> hostname >> ip >> port >> osc::EndMessage;

	//// Création de l'émetteur vers le client
	auto& sender = manager.createConnection(std::string(hostname),
												 std::string(ip),
												 port);
	emit connectionListChanged();

	//// Envoi du réseau de petri vers le client
	PetriNetSerializer ser(pnmodel.net);
	const char * cstr = ser.toFIONA();

	osc::MessageGenerator m(1024 + ser.size());
	sender.send(m("/petrinet/dump", osc::Blob(cstr, ser.size())));

	//// Envoi du pool vers le client

	auto str2 = pnmodel.pool.dump();
	sender.send(m("/pool/dump", osc::Blob(str2.c_str(), str2.size())));
}

void MainWindow::handleTake(osc::ReceivedMessageArgumentStream m)
{
	osc::int32 idRemote;
	osc::int32 idNode;

	m >> idRemote >> idNode >> osc::EndMessage;

	auto& client = manager[idRemote];

	// Vérifier si la node est bien dans le pool
	client.take(idNode, pnmodel.pool, true);

	emit localPoolChanged();
	emit clientPoolChanged(client.id());

	//TODO update pool client
}

void MainWindow::handleGive(osc::ReceivedMessageArgumentStream m)
{
	osc::int32 idRemote;
	osc::int32 idNode;

	m >> idRemote >> idNode >> osc::EndMessage;

	auto& client = manager[idRemote];

	// Vérifier si la node est bien dans le pool
	client.give(idNode, pnmodel.pool, true);

	emit localPoolChanged();
	emit clientPoolChanged(client.id());

	//TODO update pool client
}

