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
	ui->petriNetView->setModel(pnm);
}

void MasterView::setOscManager(OSCConnectionManager& osc)
{
	pOscmgr = &osc;
}

void MasterView::updateConnectionList()
{
	auto& map = pOscmgr->getMap();

	for(auto& e : map)
		ui->clientList->addItem(QString::fromStdString(e.first));
}
