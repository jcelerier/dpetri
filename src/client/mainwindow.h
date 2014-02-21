#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zeroconfconnectdialog.h"
#include <gui/petrinetmodel.h>
#include <clientmanager.h>
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
		void handleIdReception(osc::ReceivedMessageArgumentStream args);

	public slots:
		void openConnectionDialog();
		void newConnection(QHostAddress ip, quint16 port);

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;


		OscReceiver receiver{OscReceiver(9877)};

		PetriNetModel pnmodel;
		ClientManager connectMgr;

		unsigned int localId;
};

#endif // MAINWINDOW_H
