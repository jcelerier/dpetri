#include "clientview.h"
#include "ui_clientview.h"

#include <mainwindow.h>

#include <QDebug>
#include <QTime>
ClientView::ClientView(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ClientView)
{
	ui->setupUi(this);
	connect(ui->ClientToPool, SIGNAL(clicked()),
			this,			  SLOT(clientToPool()));
	connect(ui->PoolToClient, SIGNAL(clicked()),
			this,			  SLOT(poolToClient()));
}

void ClientView::updateServerPool()
{

	// Chercher les nodes du serveur
	auto& server = _logic->remoteClients[0];
	qDebug() << server.pool().size();
	ui->poolNodeList->clear();
	for(OwnedNode& e : server.pool())
	{
		ui->poolNodeList->addItem(QString::fromStdString(e.node->getName()));
	}

}

void ClientView::updateLocalPool()
{
	ui->localNodeList->clear();
	for(const OwnedNode& e : _logic->localClient.pool())
	{
		ui->localNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void ClientView::clientToPool()
{
	auto selectedNodes = ui->localNodeList->selectedItems();
	for(QListWidgetItem* item : selectedNodes)
	{
		emit give(item->text());
	}
}

void ClientView::poolToClient()
{
	auto selectedNodes = ui->poolNodeList->selectedItems();
	for(QListWidgetItem* item : selectedNodes)
	{
		emit take(item->text());
	}
}

ClientView::~ClientView()
{
	delete ui;
}

void ClientView::setClientLogic(AndroidClientLogic* logic)
{
	_logic = logic;
}

void ClientView::addLog(QString s)
{
	auto t = QTime::currentTime();
	ui->logger->append("[" + t.toString() + ":" + QString::number(t.msec()) + "] " + s);
}
