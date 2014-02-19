#include <QApplication>
#include <QTime>

#include "clock.h"
#include "clockserver.h"
#include "clockclient.h"
#include "oscreceiver.h"
#include "oscsender.h"

#include "server.h"
#include "mainwindow.h"

#include "petrinetstuff.h"
void serverTest()
{
	Clock c(500);
	OscSender sender("127.0.0.1", 9876);
	ClockServer s(c, sender);
	c.run();
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow win;
	win.show();

	//Server server;
	//server.show();
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	//int x = server.exec();
	return app.exec();
}
