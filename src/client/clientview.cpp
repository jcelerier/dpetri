#include "clientview.h"
#include "ui_clientview.h"

ClientView::ClientView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ClientView)
{
	ui->setupUi(this);
	connect(ui->ClientToPool, SIGNAL(clicked()), this, SLOT(clientToPool()));
	connect(ui->PoolToClient, SIGNAL(clicked()), this, SLOT(poolToClient()));
}

void ClientView::updatePool()
{
	ui->poolNodeList->clear();
	for(OwnedNode& e : model->pool.nodes)
	{
		ui->poolNodeList->addItem(QString::fromStdString(e.node->getName()));
	}
}

void ClientView::clientToPool()
{

}

void ClientView::poolToClient()
{
	auto selectedNodes = ui->poolNodeList->selectedItems();

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
