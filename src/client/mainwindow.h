#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zeroconfconnectdialog.h"
#include <oscconnectionmanager.h>
#include <oscreceiver.h>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void openConnectionDialog();
		void newConnection(QHostAddress ip, quint16 port);

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;
		OSCConnectionManager connectMgr;
		OscReceiver receiver{OscReceiver(9877)};
};

#endif // MAINWINDOW_H
