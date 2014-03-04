#pragma once

#include <QObject>
#include <QString>
#include <QNetworkInterface>
#include <QHostInfo>
#include <functional>
#include <chrono>

#include "clientmanager.h"
#include "client/localclient.h"

#include "type_helper.h"

class CommonLogicBase : public QObject
{
		Q_OBJECT
	public:
		CommonLogicBase(QObject* parent):
			QObject(parent)
		{
		}

		virtual ~CommonLogicBase() = default;
	signals:
		void serverPoolChanged();
		void localNetChanged();
		void localPoolChanged();
		void clientPoolChanged(int id);

		void sendLog(QString);

	private:
		void __donotuse() { } // Est utilisé pour la vtable

};

template<class PetriNetImpl>
class CommonLogic : public CommonLogicBase
{
	private:
		long int _msCount;

	public:
		ClientManager<PetriNetImpl> remoteClients;
		LocalClient<PetriNetImpl> localClient;

		CommonLogic(int id, std::string name, int port, QObject* parent):
			CommonLogicBase(parent),
			localClient(id, // Est assigné par serveur
						name,
						getIp(QHostAddress::LocalHost).toStdString(),
						port,
						std::bind(&CommonLogic<PetriNetImpl>::localNetChanged, this),
						std::bind(&CommonLogic<PetriNetImpl>::localPoolChanged, this))
		{
			//// Réseau de Petri
			localClient.receiver().addHandler("/petrinet/dump",
											  &LocalClient<PetriNetImpl>::handlePetriNetReception, &localClient);
			localClient.receiver().addHandler("/petrinet/addToken",
											  &CommonLogic<PetriNetImpl>::handleAddToken, this);
			localClient.receiver().addHandler("/petrinet/removeToken",
											  &CommonLogic<PetriNetImpl>::handleRemoveToken, this);
			localClient.receiver().addHandler("/petrinet/tokenInfo",
											  &CommonLogic<PetriNetImpl>::handleTokenInfo, this);
			//// Pool
			localClient.receiver().addHandler("/pool/dump",
											  &CommonLogic<PetriNetImpl>::handlePoolDump, this);
			localClient.receiver().addHandler("/pool/take",
											  &CommonLogic<PetriNetImpl>::handleTake, this);
			localClient.receiver().addHandler("/pool/give",
											  &CommonLogic<PetriNetImpl>::handleGive, this);
			localClient.receiver().addHandler("/pool/infoTransfer",
											  &CommonLogic<PetriNetImpl>::handleInfoTransfer, this);
			localClient.receiver().addHandler("/pool/ackTake",
											  &CommonLogic<PetriNetImpl>::handleAckTake, this);
			localClient.receiver().addHandler("/pool/ackGive",
											  &CommonLogic<PetriNetImpl>::handleAckGive, this);

			localClient.receiver().addHandler("/execution/start",
											  &CommonLogic<PetriNetImpl>::handleStart, this);

			localClient.clock().addHandle(std::bind(&CommonLogic<PetriNetImpl>::checkTransitions,
													this,
													std::placeholders::_1));
		}


#include "commonlogic.handlers.h"
#include "commonlogic.algorithm.h"

		void takeNode(QString s)
		{
			// Faire un take vers le pool du client d'id 0 (le serveur)
			auto& server = remoteClients[0];
			auto& node = server.pool()[s.toStdString()];

			server.send("/pool/take",
						(osc::int32) localClient.id(),
						(osc::int32) node.id);
		}

		void giveNode(QString s)
		{
			// Faire un give vers le pool du client d'id 0 (le serveur)
			auto& server = remoteClients[0];
			auto& node = localClient.pool()[s.toStdString()];

			server.send("/pool/give",
						(osc::int32) localClient.id(),
						(osc::int32) node.id);
		}

	private:
		QString getIp(QHostAddress serverIP)
		{
			// Cas local
			if(serverIP == QHostAddress::LocalHost) return "127.0.0.1";

			// Cas distant
			QList<QHostAddress> list = QNetworkInterface::allAddresses();

			for(int nIter=0; nIter<list.count(); nIter++)
			{
				if(!list[nIter].isLoopback())
					if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
						return list[nIter].toString();
			}

			return "0.0.0.0";
		}

		std::thread _runThread;
};
