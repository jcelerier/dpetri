#pragma once
#include "commonlogic/commonlogic.h"

class MasterView;

class ServerLogic : public CommonLogic<pnapi::PetriNet>
{
		Q_OBJECT
		friend class MasterView;
	signals:
		void connectionListChanged();

	public slots:
		void sendPlay();

	public:
		ServerLogic(QObject* parent);
		virtual ~ServerLogic() = default;
		void handleConnection(osc::ReceivedMessageArgumentStream args);
};
