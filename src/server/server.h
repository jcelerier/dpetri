#pragma once
#include <QtNetwork/QTcpServer>

class MainWindow;
class BonjourServiceRegister;

class Server : public QObject
{
		Q_OBJECT
	public:
		Server(MainWindow *parent = 0);

	private slots:
		void sendConnectionData();

	private:
		QTcpServer *tcpServer;
		BonjourServiceRegister *bonjourRegister;
};
