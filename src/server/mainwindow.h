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

	public slots:
		void handleConnection(osc::ReceivedMessageArgumentStream);

	signals:
		void connectionListChanged();

	private:
		Ui::MainWindow *ui;
		Server* server;
		OscReceiver receiver{OscReceiver(9876)};

		PetriNetModel pnmodel;
		ClientManager manager;
};

#endif // MAINWINDOW_H
