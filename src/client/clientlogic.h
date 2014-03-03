#ifndef CLIENTLOGIC_H
#define CLIENTLOGIC_H

#include "client/localclient.h"
#include "clientmanager.h"

#include "commonlogic/commonlogic.h"

class ClientView;
class MainWindow;
class ClientLogic : public CommonLogic
{
		Q_OBJECT
		friend class ClientView;

	public:
		ClientLogic(MainWindow* parent);
		virtual ~ClientLogic() { }

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);


};

#endif // CLIENTLOGIC_H
