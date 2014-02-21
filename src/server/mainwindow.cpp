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

	connect(this,			   SIGNAL(connectionListChanged()),
			ui->centralwidget, SLOT(updateConnectionList()));

	connect(ui->centralwidget, SIGNAL(play()),
			&pnmodel,		   SLOT(start()));

	receiver.addHandler("/connect",
						std::bind(&MainWindow::handleConnection, this, std::placeholders::_1));

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

