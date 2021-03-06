#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <array>
#include <functional>
#include <sstream>
#include <QtNetwork/QNetworkInterface>
#include <osc/OscOutboundPacketStream.h>
#include "osc/oscmessagegenerator.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	logic(this),
	connectDialog(new ZeroconfConnectDialog(this))
{
	ui->setupUi(this);

	ui->centralwidget->setClientLogic(&logic);

	connect(connectDialog, SIGNAL(setLocalAddress(QHostAddress)),
			&logic,			 SLOT(setLocalAddress(QHostAddress)));

	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newServerConnection(QHostAddress,quint16)));

	connect(ui->centralwidget, SIGNAL(take(QString)),
			this,			   SLOT(takeNode(QString)));

	connect(ui->centralwidget, SIGNAL(give(QString)),
			this,			   SLOT(giveNode(QString)));

	connect(&logic,			   SIGNAL(localNetChanged()),
			ui->centralwidget, SLOT(updateNet()));

	connect(&logic,			   SIGNAL(serverPoolChanged()),
			ui->centralwidget, SLOT(updateServerPool()));

	connect(&logic,			   SIGNAL(localPoolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	connect(&logic,			   SIGNAL(sendLog(QString)),
			ui->centralwidget, SLOT(addLog(QString)));
}

MainWindow::~MainWindow()
{
	delete ui;
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

	auto& sender = logic.remoteClients.createConnection("server", ip.toString().toStdString(), port);

	sender.initConnectionTo(logic.localClient);
}
