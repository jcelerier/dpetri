#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>
#include <QCoreApplication>
#include <sstream>
#include <osctools.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new ZeroconfServer(this)),
	logic(this)
{
	ui->setupUi(this);
	ui->centralwidget->setServerLogic(&logic);

	connect(ui->actionLoad_a_Petri_Net, SIGNAL(triggered()),
			this,					SLOT(loadNetAndPoolFromFile()));

	connect(this,			   SIGNAL(connectionListChanged()),
			ui->centralwidget, SLOT(updateConnectionList()));

	connect(this,			   SIGNAL(localPoolChanged()),
			ui->centralwidget, SLOT(updateLocalPool()));

	connect(this,			   SIGNAL(clientPoolChanged(int)),
			ui->centralwidget, SLOT(updateClientPool(int)));

	connect(this, SIGNAL(localNetChanged()),
			ui->centralwidget, SLOT(updateNet()));


/*	receiver.addHandler("/petrinet/addToken",
						std::bind(&PetriNetModel::handleAddToken,
								  pnmodel,
								  std::placeholders::_1));

	receiver.addHandler("/petrinet/removeToken",
						std::bind(&PetriNetModel::handleRemoveToken,
								  pnmodel,
								  std::placeholders::_1));
*/

	auto x = QCoreApplication::arguments();
	if(x.size() > 1)
	{
		loadFromFile(x.at(1));
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadNetAndPoolFromFile()
{
	QString file = QFileDialog::getOpenFileName(nullptr,
												tr("Choose a Petri Net"),
												QString(),
												tr("FIONA File (*.fiona)"));
	loadFromFile(file);
}


void MainWindow::loadFromFile(QString s)
{
	std::ifstream f(s.toStdString());
	logic.localClient.loadNetAndPool(f);
}

