#include "masterview.h"
#include "ui_masterview.h"

#include "mainwindow.h"

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
