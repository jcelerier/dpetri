#include "clientview.h"
#include "ui_clientview.h"

ClientView::ClientView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ClientView)
{
	ui->setupUi(this);
}

ClientView::~ClientView()
{
	delete ui;
}
