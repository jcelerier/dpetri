#include "clock.h"
#include "clockserver.h"
#include "clockclient.h"
#include "oscreceiver.h"


#include <QApplication>

#include "zeroconfconnectdialog.h"
#include "mainwindow.h"

void clientTest()
{
	OscReceiver r(9876);
	Clock c;
	ClockClient h(c);
	auto f = std::bind(&ClockClient::clockMessage, &h, std::placeholders::_1);
	r.addHandler("/clock/send", f);
	r.run();
}



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow w;
	w.show();
	return app.exec();
}
