#include "masterview.h"
#include "ui_masterview.h"

#include "mainwindow.h"
#include <ownednode.h>


MasterView::MasterView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MasterView)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(play()));
}

MasterView::~MasterView()
{
	delete ui;
}

void MasterView::setServerLogic(ServerLogic* cl)
{
	_logic = cl;
}

void MasterView::updateConnectionList()
{
	auto& clients = _logic->remoteClients.clients();

	ui->clientList->clear();
	for(auto& c : clients)
		ui->clientList->addItem(QString::fromStdString(c.name()));
}

void MasterView::updateClientPool(int id)
{
	auto list = ui->clientList->selectedItems();
	if(list.isEmpty()) return;

	auto text = list.first()->text().toStdString();
	auto& client = _logic->remoteClients[id];

	if(client.name() != text) return;

	ui->clientNodeList->clear();
	for(OwnedNode& e : client.pool())
	{
		ui->clientNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void MasterView::updateLocalPool()
{
	ui->localNodeList->clear();
	for(OwnedNode& e : _logic->localClient.pool())
	{
		ui->localNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void MasterView::onClientSelection(QListWidgetItem* selected)
{
	auto text = selected->text().toStdString();
	auto& client = _logic->remoteClients[text];

	ui->clientNodeList->clear();
	for(OwnedNode& e : client.pool())
	{
		ui->clientNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void MasterView::updateNet()
{
	ui->petriNetView->updatePetriNet(_logic->localClient.model().net());
}

