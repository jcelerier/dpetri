#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <oscreceiver.h>
#include <oscconnectionmanager.h>
#include "server.h"
namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT
		friend class Server;

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void handleConnection(osc::ReceivedMessageArgumentStream);

	private:
		Ui::MainWindow *ui;
		Server* server;
		OscReceiver receiver{OscReceiver(9876)};

		OSCConnectionManager manager;
};

#endif // MAINWINDOW_H
