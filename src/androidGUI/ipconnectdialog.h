#ifndef IPCONNECTDIALOG_H
#define IPCONNECTDIALOG_H
#include <QHostAddress>
#include <QTcpSocket>
#include <QDialog>

namespace Ui {
	class IPConnectDialog;
}

class IPConnectDialog : public QDialog
{
		Q_OBJECT

	signals:
		void connectedTo(QHostAddress, quint16);
		void setLocalAddress(QHostAddress);
	public:
		explicit IPConnectDialog(QWidget *parent = 0);
		~IPConnectDialog();

	public slots:
		void accept();
		void readConnectionData();
	private:
		Ui::IPConnectDialog *ui;
		QTcpSocket tcpSocket;
};

#endif // IPCONNECTDIALOG_H
