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

		void handleTake(osc::ReceivedMessageArgumentStream m)
		{
			osc::int32 idRemote;
			osc::int32 idNode;

			m >> idRemote >> idNode >> osc::EndMessage;
			qDebug() << idRemote << idNode;

			auto it = std::find_if(manager.clients().begin(),
								   manager.clients().end(),
								   [idRemote] (RemoteClient& c )
										{ return c.id() == idRemote; });

			if(it != manager.clients().end())
			{
				// Vérifier si la node est bien dans le pool
				it->take(idNode, pnmodel.pool);
			}
		}

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
