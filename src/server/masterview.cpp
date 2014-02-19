#include "masterview.h"
#include "ui_masterview.h"

#include "mainwindow.h"

MasterView::MasterView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MasterView)
{
	ui->setupUi(this);
}

MasterView::~MasterView()
{
	delete ui;
}

void MasterView::setPetriNetModel(PetriNetModel& pnm)
{
	ui->petriNetView->setModel(pnm);
}

void MasterView::updateConnectionList()
{
	auto map = dynamic_cast<MainWindow*>(parent())->manager.getMap();

	for(auto i = map.begin(); i != map.end(); ++i)
		ui->clientList->addItem(QString::fromStdString(i->first));
}
