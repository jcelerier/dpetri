#include "mainapplication.h"
#include "ui_mainapplication.h"

MainApplication::MainApplication(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainApplication),
	logic(this)
{
	ui->setupUi(this);
}

MainApplication::~MainApplication()
{
	delete ui;
}
