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
class ClientView;
class MainWindow : public QMainWindow
{
		Q_OBJECT
		friend class ClientView;

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);
		void handlePetriNetReception(osc::ReceivedMessageArgumentStream args);
		void handleIdReception(osc::ReceivedMessageArgumentStream args);
		void handleAckTake(osc::ReceivedMessageArgumentStream args);
		void handleAckGive(osc::ReceivedMessageArgumentStream args);
		void handlePoolDump(osc::ReceivedMessageArgumentStream args);

	signals:
		void poolChanged();
		void localPoolChanged();

	public slots:
		void openConnectionDialog();
		void newServerConnection(QHostAddress ip, quint16 port);

		void takeNode(QString s);
		void giveNode(QString s);

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;

		OscReceiver receiver{9877};

		PetriNetModel pnmodel;
		ClientManager clientMgr;
		Client localClient;

		unsigned int localId;
};

#endif // MAINWINDOW_H
