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

void MasterView::setPetriNetModel(PetriNetModel& pnm)
{
	model = &pnm;
	ui->petriNetView->setModel(pnm);
}

void MasterView::setOscManager(ClientManager& osc)
{
	pOscmgr = &osc;
}

void MasterView::updateConnectionList()
{
	auto& clients = pOscmgr->clients();

	for(auto& c : clients)
		ui->clientList->addItem(QString::fromStdString(c.name()));
}

void MasterView::updateClientPool(int id)
{
	auto list = ui->clientList->selectedItems();
	if(list.isEmpty()) return;

	auto text = list.first()->text().toStdString();
	auto& client = (*pOscmgr)[id];

	if(client.name() != text) return;

	ui->clientNodeList->clear();
	for(OwnedNode& e : client.pool().nodes)
	{
		ui->clientNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void MasterView::updateLocalPool()
{
	ui->localNodeList->clear();
	for(OwnedNode& e : model->pool.nodes)
	{
		ui->localNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void MasterView::onClientSelection(QListWidgetItem* selected)
{
	auto text = selected->text().toStdString();
	auto& client = (*pOscmgr)[text];

	ui->clientNodeList->clear();
	for(OwnedNode& e : client.pool().nodes)
	{
		ui->clientNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}
