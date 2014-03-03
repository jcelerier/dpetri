#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientlogic.h"
#include <QHostAddress>
#include "zeroconfconnectdialog.h"
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

	public slots:
		void openConnectionDialog();
		void newServerConnection(QHostAddress ip, quint16 port);

	private:
		Ui::MainWindow *ui;

		ClientLogic logic;

		ZeroconfConnectDialog* connectDialog;

};

#endif // MAINWINDOW_H
