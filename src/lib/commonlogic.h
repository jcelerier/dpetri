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
			localClient.receiver().addHandler("/pool/take",
											  &CommonLogic::handleTake, this);

			localClient.receiver().addHandler("/pool/give",
											  &CommonLogic::handleGive, this);
		}

		void handleAckTake(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 id;
			osc::int32 nodeId;

			args >> id >> nodeId >> osc::EndMessage;

			remoteClients[id].give(nodeId, localClient.pool());

			emit serverPoolChanged();
			emit localPoolChanged();

			std::cerr << "Number  of clients: " << remoteClients.clients().size() << std::endl;

			for(RemoteClient& c : remoteClients)
			{
				if(c.id() != 0) // Pas le serveur (why not ?)
					localClient.pool().dumpTo(localClient.id(), c);
			}

			// Potentiellement unifier en rajoutant un message
			// "/pool/confirm id-from id-to id-node"
			// que chacun applique ?
			// -> plus besoin d'envoyer les pool (mais cas ou ils ne communiquent pas ?)
		}


		void handleAckGive(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 id;
			osc::int32 nodeId;

			args >> id >> nodeId >> osc::EndMessage;

			remoteClients[id].take(nodeId, localClient.pool());

			emit serverPoolChanged();
			emit localPoolChanged();

			for(RemoteClient& c : remoteClients)
			{
				if(c.id() != 0) // Pas le serveur (il le fait, changer?)
					localClient.pool().dumpTo(localClient.id(), c);
			}
		}

		void handlePoolDump(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 id;
			osc::Blob b;
			args >> id >> b >> osc::EndMessage;

			remoteClients[id].pool().loadFromString(localClient.model().net(),
													static_cast<const char*>(b.data));

			if(id == 0) emit serverPoolChanged(); // Serveur
		}

		void removeToken(std::string name)
		{
			// Trouver qui possède cette node (ou broadcast ?)
			if(localClient.pool().hasNode(name)) // Cas local
			{
				auto p = dynamic_cast<Place*>(localClient.pool()[name].node);
				if(p)
				{
					p->setTokenCount(p->getTokenCount() - 1);
				}
				return;
			}
			else
			{
				// chercher dans tous les pools clients
				for(RemoteClient& c : remoteClients)
				{
					if(c.pool().hasNode(name))
					{
						c.send("/net/removeToken", name.c_str());
						return;
					}
				}
			}

			throw "Error: nobody has this node!";
		}

		void handleAddToken(osc::ReceivedMessageArgumentStream args)
		{
			const char * tname;
			args >> tname >> osc::EndMessage;

			auto place = dynamic_cast<Place*>(localClient.pool()[std::string(tname)].node);
			place->setTokenCount(place->getTokenCount()+1);

			updateRemotePlace(place);
		}

		void handleRemoveToken(osc::ReceivedMessageArgumentStream args)
		{
			const char * tname;
			args >> tname >> osc::EndMessage;

			auto place = dynamic_cast<Place*>(localClient.pool()[std::string(tname)].node);
			place->setTokenCount(place->getTokenCount()-1);
		}

		void handleTokenInfo(osc::ReceivedMessageArgumentStream args)
		{
			const char* pname;
			int pcount;

			args >> pname >> pcount;

			// Chercher le pool ou est pname.
			auto p = localClient.model().net().findPlace(std::string(pname));
			p->setTokenCount(pcount);
		}

		// Fonction d'exécution de transition.
		void executeTransition(Transition* t)
		{
			// 1. Vérifier qu'antécédents ont jetons est fait avant
			for(Node* n : t->getPreset())
			{
				removeToken(n->getName());
			}

			// 2. Envoyer nouveau jeton aux suivants
			for(Node* n : t->getPostset())
			{
				addToken(n->getName());
			}
		}

		// Quand on reçoit un jeton sur une place, on en informe la machine gérant la transitions suivantes
		// pour qu'elles puissent chercher.
		// Ou alors les messages de transmission de jeton sont transmis à tout le monde ?
		void updateRemotePlace(Place* place)
		{
			auto t = dynamic_cast<Transition*>(*place->getPostset().begin());

			// Trouver qui possède cette transition (ou broadcast ?)
			if(!localClient.pool().hasNode(t->getName()))
			{
				// chercher dans tous les pools clients
				for(RemoteClient& c : remoteClients)
				{
					if(c.pool().hasNode(t->getName()))
					{
						c.send("/petrinet/tokenInfo",
							   place->getName().c_str(),
							   (osc::int32) place->getTokenCount());
					}
				}
			}
		}



		void addToken(std::string name)
		{
			// Trouver qui possède cette node (ou broadcast ?)
			if(localClient.pool().hasNode(name)) // Cas local
			{
				auto p = dynamic_cast<Place*>(localClient.pool()[name].node);
				if(p)
				{
					p->setTokenCount(p->getTokenCount() + 1);
				}
				return;
			}
			else
			{
				// chercher dans tous les pools clients
				for(RemoteClient& c : remoteClients)
				{
					if(c.pool().hasNode(name))
					{
						c.send("/petrinet/addToken", name.c_str());
						return;
					}
				}
			}

			throw "Error: nobody has this node!";
		}

		void algo()
		{
			// Attendre ticks d'horloge
			// à chaque tick, regarder si on peut exécuter une des transitions
			// que l'on possède
			for(Transition* t : localClient.model().net().getTransitions())
			{
				if(localClient.pool().hasNode(t->getName()))
				{
					if(std::all_of(t->getPreset().begin(),
								   t->getPreset().end(),
								   [] (Node* n)
					{
								   return dynamic_cast<Place*>(n)->getTokenCount() > 0;
				})
							&& localClient.clock().get() > t->getCost()
							)
					{
						executeTransition(t);
					}
				}
			}
		}


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


		void handleTake(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 idRemote;
			osc::int32 idNode;

			args >> idRemote >> idNode >> osc::EndMessage;

			auto& client = remoteClients[idRemote];

			// Vérifier si la node est bien dans le pool
			client.take(idNode, localClient.pool());
			client.send("/pool/ackTake",
						0,
						(osc::int32) idNode); // Cas serveur

			//Mise-à-jour de l'image du pool serveur des autres clients
			for(RemoteClient& c : remoteClients)
			{
				if(c.id() != idRemote)
					localClient.pool().dumpTo(0, c);
			}

			emit localPoolChanged();
			emit clientPoolChanged(client.id());
		}

		void handleGive(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 idRemote;
			osc::int32 idNode;

			args >> idRemote >> idNode >> osc::EndMessage;

			auto& client = remoteClients[idRemote];

			// Vérifier si la node est bien dans le pool
			client.give(idNode, localClient.pool());
			client.send("/pool/ackGive",
						0,
						(osc::int32) idNode);

			//Mise-à-jour des pools des autres clients
			for(RemoteClient& c : remoteClients)
			{
				if(c.id() != idRemote)
					localClient.pool().dumpTo(0, c);
			}

			emit localPoolChanged();
			emit clientPoolChanged(client.id());
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
};




