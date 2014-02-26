#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <oscreceiver.h>
#include <clientmanager.h>
#include "server.h"
#include <gui/petrinetmodel.h>

namespace Ui {
	class MainWindow;
}

class MasterView;
class MainWindow : public QMainWindow
{
		Q_OBJECT
		friend class Server;
		friend class MasterView;

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		// OscPack handlers
		void handleConnection(osc::ReceivedMessageArgumentStream m);
		void handleTake(osc::ReceivedMessageArgumentStream m);
		void handleGive(osc::ReceivedMessageArgumentStream m);

	signals:
		void connectionListChanged();
		void localPoolChanged();
		void clientPoolChanged(int id);

	private:
		Ui::MainWindow *ui;
		Server* server;
		OscReceiver receiver{7000};

		PetriNetModel pnmodel;
		ClientManager manager;
};

#endif // MAINWINDOW_H
