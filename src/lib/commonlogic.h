#pragma once

#include <QObject>
#include <QString>
#include <QNetworkInterface>
#include <QHostInfo>
#include <functional>

#include "clientmanager.h"
#include "client/localclient.h"

class CommonLogic : public QObject
{
		Q_OBJECT
	signals:
		void serverPoolChanged();
		void localNetChanged();
		void localPoolChanged();
		void clientPoolChanged(int id);


	public:
		ClientManager remoteClients;
		LocalClient localClient;

		CommonLogic(int id, std::string name, int port, QObject* parent):
			QObject(parent),
			localClient(id, // Est assigné par serveur
						name,
						getIp(QHostAddress::LocalHost).toStdString(),
						port,
						std::bind(&CommonLogic::localNetChanged, this),
						std::bind(&CommonLogic::localPoolChanged, this))
		{
			//// Réseau de Petri
			localClient.receiver().addHandler("/petrinet/dump",
											  &LocalClient::handlePetriNetReception, &localClient);
			localClient.receiver().addHandler("/petrinet/addToken",
											  &CommonLogic::handleAddToken, this);
			localClient.receiver().addHandler("/petrinet/removeToken",
											  &CommonLogic::handleRemoveToken, this);
			localClient.receiver().addHandler("/petrinet/tokenInfo",
											  &CommonLogic::handleTokenInfo, this);
			//// Pool
			localClient.receiver().addHandler("/pool/dump",
											  &CommonLogic::handlePoolDump, this);
			localClient.receiver().addHandler("/pool/take",
											  &CommonLogic::handleTake, this);
			localClient.receiver().addHandler("/pool/give",
											  &CommonLogic::handleGive, this);
			localClient.receiver().addHandler("/pool/infoTransfer",
											  &CommonLogic::handleInfoTransfer, this);
			localClient.receiver().addHandler("/pool/ackTake",
											  &CommonLogic::handleAckTake, this);
			localClient.receiver().addHandler("/pool/ackGive",
											  &CommonLogic::handleAckGive, this);

			localClient.receiver().addHandler("/execution/start",
											  &CommonLogic::handleStart, this);

			localClient.clock().addHandle(std::bind(&CommonLogic::checkTransitions, this, std::placeholders::_1));

		}

		// Renvoie le client qui posède la node d'id // nom voulu
		Client& getClientWithNode(std::string name)
		{
			if(localClient.pool().hasNode(name)) return localClient;

			for(RemoteClient& c : remoteClients)
			{
				if(c.pool().hasNode(name)) return c;
			}

			throw "Node not found";
		}

		void startAlgorithm()
		{
			localClient.clock().start();
		}

#include "commonlogic.handlers.h"
#include "commonlogic.algorithm.h"

	public slots:
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




