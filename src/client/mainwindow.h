#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zeroconfconnectdialog.h"
#include <gui/petrinetmodel.h>
#include <clientmanager.h>
#include <oscreceiver.h>
#include <pnapi/pnapi.h>

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

		void handlePetriNetReception(osc::ReceivedMessageArgumentStream args);
		void handleIdReception(osc::ReceivedMessageArgumentStream args);

		void handleAckTake(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 id;
			osc::int32 nodeId;

			args >> id >> nodeId >> osc::EndMessage;

			qDebug() << id << nodeId;

			// On doit faire passer node de pool du serveur vers pool local
			// Obtenir it vers pool du serveur :
			auto it = std::find_if(clientMgr.clients().begin(),
								   clientMgr.clients().end(),
								   [] (RemoteClient& cl)
			{ return cl.id() == 0; });

			if(it != clientMgr.clients().end())
			{
				// Trouver la node d'id nodeId
				auto nit = std::find_if(it->pool().nodes.begin(),
										it->pool().nodes.end(),
										[&nodeId] (OwnedNode& n)
				 { return n.id == nodeId; });
				pnmodel.pool.nodes.splice(pnmodel.pool.nodes.begin(), it->pool().nodes, nit);

				emit poolChanged();
				emit localPoolChanged();
			}
		}

		void handleDump(osc::ReceivedMessageArgumentStream args);

	signals:
		void poolChanged();
		void localPoolChanged();

	public slots:
		void openConnectionDialog();
		void newConnection(QHostAddress ip, quint16 port);


		void takeNode(QString s);

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;


		OscReceiver receiver{OscReceiver(9877)};

		PetriNetModel pnmodel;
		ClientManager clientMgr;

		unsigned int localId;
};

#endif // MAINWINDOW_H
