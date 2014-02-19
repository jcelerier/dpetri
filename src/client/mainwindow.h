#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zeroconfconnectdialog.h"
#include <gui/petrinetmodel.h>
#include <oscconnectionmanager.h>
#include <oscreceiver.h>
#include <pnapi/pnapi.h>
using namespace pnapi;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		void handlePetriNetReception(osc::ReceivedMessageArgumentStream args);

	public slots:
		void openConnectionDialog();
		void newConnection(QHostAddress ip, quint16 port);

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;
		OSCConnectionManager connectMgr;
		OscReceiver receiver{OscReceiver(9877)};

		PetriNetModel pnmodel;
};

#endif // MAINWINDOW_H
