#ifndef CLIENTLOGIC_H
#define CLIENTLOGIC_H

#include "client/localclient.h"
#include "clientmanager.h"

#include "commonlogic/commonlogic.h"

class ClientView;
class MainWindow;

class ClientLogic : public CommonLogic<pnapi::PetriNet>
{
		Q_OBJECT
		friend class ClientView;

	public:
		ClientLogic(MainWindow* parent);
		virtual ~ClientLogic() { }

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);


	public slots:
		void setLocalAddress(QHostAddress ip);
};

#endif // CLIENTLOGIC_H
