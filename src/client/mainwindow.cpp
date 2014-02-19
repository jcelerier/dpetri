#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <array>
#include <functional>
#include <sstream>
#include <QtNetwork/QNetworkInterface>
#include <osc/OscOutboundPacketStream.h>

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

QString getIp()
{
	QList<QHostAddress> list = QNetworkInterface::allAddresses();

	for(int nIter=0; nIter<list.count(); nIter++)
	{
		if(!list[nIter].isLoopback())
			if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
				return list[nIter].toString();
	}

	return "NoIP";
}

void MainWindow::newConnection(QHostAddress ip, quint16 port)
{
	if(ip.toString() == QString("0.0.0.0"))
		ip = QHostAddress("127.0.0.1");

	OscSender& sender = connectMgr.createConnection("server", ip.toString().toStdString(), port);

	std::array<char, 1024> buffer;
	osc::OutboundPacketStream p(buffer.data(), 1024);

	p.Clear();

	p << osc::BeginBundleImmediate
		<< osc::BeginMessage( "/connect" )
			<< QHostInfo::localHostName().toStdString().c_str()
			<< "127.0.0.1" // getIp().toStdString().c_str()
			<< (osc::int32) receiver.port()
		<< osc::EndMessage
	  << osc::EndBundle;


	sender.send(p);
}
