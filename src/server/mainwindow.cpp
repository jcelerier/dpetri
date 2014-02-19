#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new Server(this)),
	pnmodel(this)
{
	ui->setupUi(this);
	ui->centralwidget->setPetriNetModel(pnmodel);

	connect(ui->actionLoad_a_Petri_Net, SIGNAL(triggered()),
			&pnmodel,					SLOT(loadFile()));
	connect(this,			   SIGNAL(connectionListChanged()),
			ui->centralwidget, SLOT(updateConnectionList()));

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
	OscSender& sender = manager.createConnection(std::string(hostname), std::string(ip), port);
	emit connectionListChanged();

	//// Envoi du réseau de petri vers le client
	// Sérialisation :
	std::stringstream spnet;
	spnet << io::pnml << pnmodel.net;
	std::string strpnet = spnet.str();
	char * cstr = new char[strpnet.size() + 1];
	std::copy(strpnet.begin(), strpnet.end(), cstr);
	cstr[strpnet.length()] = 0;

	std::vector<char> buffer(1024 + strpnet.size());
	osc::OutboundPacketStream p(buffer.data(), buffer.size());

	p.Clear();
	// TODO en mieux avec template variadique
	p << osc::BeginBundleImmediate
		<< osc::BeginMessage( "/petrinet/dump" )
		   << osc::Blob( cstr, strpnet.size() + 1 )
		<< osc::EndMessage
	  << osc::EndBundle;

	sender.send(p);
}
