#include <QApplication>
#include <QTime>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainWindow win;
	win.show();

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	return app.exec();
}


