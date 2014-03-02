#pragma once
#include <QtNetwork/QTcpServer>

class MainWindow;
class BonjourServiceRegister;

class ZeroconfServer : public QObject
{
		Q_OBJECT
	public:
		ZeroconfServer(MainWindow *parent = 0);

	private slots:
		void sendConnectionData();

	private:
		QTcpServer *tcpServer;
		BonjourServiceRegister *bonjourRegister;
};
