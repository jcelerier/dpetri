#include "ipconnectdialog.h"
#include "ui_ipconnectdialog.h"
#include <QLineEdit>
#include <QtWidgets/QtWidgets>
#include <QDataStream>

IPConnectDialog::IPConnectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::IPConnectDialog)
{
	ui->setupUi(this);

	connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(readConnectionData()));

}

IPConnectDialog::~IPConnectDialog()
{
	delete ui;
}

void IPConnectDialog::accept()
{
	tcpSocket.connectToHost(ui->ip->text(), ui->port->value());
	this->close();
}

void IPConnectDialog::readConnectionData()
{
	QDataStream in(&tcpSocket);
	in.setVersion(QDataStream::Qt_5_2);

	quint16 blockSize{0};
	if (blockSize == 0)
	{
		if (tcpSocket.bytesAvailable() < (int)sizeof(quint16))
			return;

		in >> blockSize;
	}

	if (tcpSocket.bytesAvailable() < blockSize)
		return;

	quint16 port;
	QHostAddress ip;
	in >> ip >> port;
	emit setLocalAddress(tcpSocket.localAddress());
	emit connectedTo(ip, port);
}

