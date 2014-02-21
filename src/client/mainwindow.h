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

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		void handlePetriNetReception(osc::ReceivedMessageArgumentStream args);
		void handleIdReception(osc::ReceivedMessageArgumentStream args);

	public slots:
		void openConnectionDialog();
		void newConnection(QHostAddress ip, quint16 port);


		void takeNode(QString s)
		{
			// Faire un take vers le pool du client d'id 0 (le serveur)
			auto it = std::find_if(connectMgr.clients().begin(),
								   connectMgr.clients().end(),
								   [] (RemoteClient& cl)
									{ return cl.id() == 0; });

			if(it != connectMgr.clients().end())
			{
				qDebug() << "coucou" << it->pool().nodes.size();

				auto node = std::find_if(it->pool().nodes.begin(),
							 it->pool().nodes.end(),
							 [&s] (OwnedNode& n)
								{ std::cerr << n.node->getName() <<" ";
								return n.node->getName() == s.toStdString(); });

				if(node != it->pool().nodes.end())
				{
					osc::MessageGenerator m;
					it->send(m("/pool/take", (osc::int32) localId, (osc::int32) node->id));
				}
			}
			// Il envoie la demande

			// A la réception de ack on applique
		}

	private:
		Ui::MainWindow *ui;
		ZeroconfConnectDialog* connectDialog;


		OscReceiver receiver{OscReceiver(9877)};

		PetriNetModel pnmodel;
		ClientManager connectMgr;

		unsigned int localId;
};

#endif // MAINWINDOW_H
