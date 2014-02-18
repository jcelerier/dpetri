#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "masterview.h"
#include <functional>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	server(new Server(this))
{
	ui->setupUi(this);
	receiver.addHandler("/connect",
						std::bind(&MainWindow::handleConnection, this, std::placeholders::_1));

	receiver.run();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::handleConnection(osc::ReceivedMessageArgumentStream args)
{
	const char* ip;
	osc::int32 port;
	args >> ip >> port >> osc::EndMessage;

	manager.createConnection(std::string(ip), port);
}
