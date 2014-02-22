#include "clientview.h"
#include "ui_clientview.h"

#include <mainwindow.h>

#include <QDebug>
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

void ClientView::updatePool()
{
	ui->poolNodeList->clear();

	// Chercher les nodes du serveur
	auto it = std::find_if(myParent->clientMgr.clients().begin(),
						   myParent->clientMgr.clients().end(),
						   [] (RemoteClient& cl)
	{ return cl.id() == 0; });

	if(it != myParent->clientMgr.clients().end())
	{
		for(OwnedNode& e : it->pool().nodes)
		{
			ui->poolNodeList->addItem(QString::fromStdString(e.node->getName()));
		}
	}
}

void ClientView::updateLocalPool()
{
	ui->localNodeList->clear();
	for(OwnedNode& e : model->pool.nodes)
	{
		ui->localNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void ClientView::clientToPool()
{

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

void ClientView::setPetriNetModel(PetriNetModel& pnm)
{
	model = &pnm;
	ui->petriNetView->setModel(pnm);
}

void ClientView::setParent(MainWindow* window)
{
	myParent = window;
}
