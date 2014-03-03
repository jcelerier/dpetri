#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "serverlogic.h"

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


	public slots:
		void loadNetAndPoolFromFile();

	private:
		Ui::MainWindow *ui;
		ZeroconfServer* server;

		ServerLogic logic;
};

#endif // MAINWINDOW_H
