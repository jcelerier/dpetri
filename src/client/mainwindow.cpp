#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <array>
#include <QtNetwork/QNetworkInterface>
#include <osc/OscOutboundPacketStream.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	connectDialog(new ZeroconfConnectDialog(this))
{
	ui->setupUi(this);
	connect(connectDialog, SIGNAL(connectedTo(QHostAddress,quint16)),
			this,		   SLOT(newConnection(QHostAddress,quint16)));
}

MainWindow::~MainWindow()
{
	delete ui;
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

	OscSender& sender = connectMgr.createConnection(ip.toString().toStdString(), port);

	std::array<char, 1024> buffer;
	osc::OutboundPacketStream p(buffer.data(), 1024);

	p.Clear();

	p << osc::BeginBundleImmediate
	  << osc::BeginMessage( "/connect" ) << getIp().toStdString().c_str() << (osc::int32) receiver.port() << osc::EndMessage
	  << osc::EndBundle;


	sender.send(p);
}