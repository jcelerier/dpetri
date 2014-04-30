#pragma once

#include "client/localclient.h"
#include "clientmanager.h"
#include "commonlogic/commonlogic.h"

#include "simpleNet/simplePetriNet.h"

class MainWindow;
class AndroidClientLogic : public CommonLogic<SimplePetriNet>
{
		Q_OBJECT
	public:
		AndroidClientLogic(MainWindow* parent);
		virtual ~AndroidClientLogic() { }

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);

	public slots:
		void setLocalAddress(QHostAddress ip);
};
