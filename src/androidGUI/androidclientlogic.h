#pragma once

#include "client/localclient.h"
#include "clientmanager.h"
#include "commonlogic/commonlogic.h"

#include "simpleNet/simplePetriNet.h"

class MainApplication;
class AndroidClientLogic : public CommonLogic<SimplePetriNet>
{
		Q_OBJECT
	public:
		AndroidClientLogic(MainApplication* parent);
		virtual ~AndroidClientLogic() { }

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);


};
