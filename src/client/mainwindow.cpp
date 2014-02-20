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
	pnmodel(this)
{
	ui->setupUi(this);

	ui->centralwidget->setPetriNetModel(pnmodel);

	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newConnection(QHostAddress,quint16)));

	receiver.addHandler("/petrinet/dump",
						std::bind(&MainWindow::handlePetriNetReception,
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

	OscSender& sender = connectMgr.createConnection("server", ip.toString().toStdString(), port);

	osc::MessageGenerator m;
	sender.send(m("/connect",
				  QHostInfo::localHostName().toStdString().c_str(),
				  getIp(ip).toStdString().c_str(),
				  (osc::int32) receiver.port()));
}
