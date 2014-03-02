#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <oscreceiver.h>
#include <clientmanager.h>
#include "server.h"
#include <client/localclient.h>

namespace Ui {
	class MainWindow;
}

class MasterView;
class MainWindow : public QMainWindow
{
		Q_OBJECT
		friend class ZeroconfServer;
		friend class MasterView;

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		void loadFromFile(QString s);

		// OscPack handlers
		void handleConnection(osc::ReceivedMessageArgumentStream m);
		void handleTake(osc::ReceivedMessageArgumentStream m);
		void handleGive(osc::ReceivedMessageArgumentStream m);

	signals:
		void connectionListChanged();
		void localNetChanged();
		void localPoolChanged();
		void clientPoolChanged(int id);

	public slots:
		void loadNetAndPoolFromFile();

	private:
		Ui::MainWindow *ui;
		ZeroconfServer* server;

		ClientManager clientMgr;
		LocalClient localClient;
};

#endif // MAINWINDOW_H
