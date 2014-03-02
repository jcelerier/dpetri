#pragma once
#include "commonlogic.h"

class MasterView;

class ServerLogic : public CommonLogic
{
		Q_OBJECT
		friend class MasterView;
	signals:
		void connectionListChanged();
	public:
		ServerLogic(QObject* parent);
		void handleConnection(osc::ReceivedMessageArgumentStream args);
};
