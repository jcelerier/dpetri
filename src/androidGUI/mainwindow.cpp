#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkInterface>
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	logic(this),
	connectDialog(new ZeroconfConnectDialog(this)),
	ipConnectDialog(new IPConnectDialog(this))
{
	ui->setupUi(this);

	ui->centralwidget->setClientLogic(&logic);
/*
	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newServerConnection(QHostAddress,quint16)));
*/
	connect(ipConnectDialog, SIGNAL(setLocalAddress(QHostAddress)),
			&logic,			 SLOT(setLocalAddress(QHostAddress)));

	connect(ipConnectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newServerConnection(QHostAddress,quint16)));



	connect(ui->centralwidget, SIGNAL(take(QString)),
			this,			   SLOT(takeNode(QString)));

	connect(ui->centralwidget, SIGNAL(give(QString)),
			this,			   SLOT(giveNode(QString)));

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
	ipConnectDialog->exec();
	//connectDialog->exec();
}

void MainWindow::newServerConnection(QHostAddress ip, quint16 port)
{

	qDebug() << "Here" << ip << port;
	// Connection via zeroconf
	auto& sender = logic.remoteClients.createConnection("server", ip.toString().toStdString(), port);
	sender.initConnectionTo(logic.localClient);
}
