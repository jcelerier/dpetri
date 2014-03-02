#ifndef CLIENTLOGIC_H
#define CLIENTLOGIC_H

#include "client/localclient.h"
#include "clientmanager.h"
#include <QObject>
#include <QString>


class ClientView;
class MainWindow;
class ClientLogic : public QObject
{
		Q_OBJECT
		friend class ClientView;

	signals:
		void serverPoolChanged();
		void localNetChanged();
		void localPoolChanged();

	public slots:
		void takeNode(QString s);
		void giveNode(QString s);

	public:
		ClientLogic(MainWindow* parent);
		virtual ~ClientLogic() { }

		void handleConnectDiscover(osc::ReceivedMessageArgumentStream args);
		void handleAckTake(osc::ReceivedMessageArgumentStream args);
		void handleAckGive(osc::ReceivedMessageArgumentStream args);
		void handlePoolDump(osc::ReceivedMessageArgumentStream args);

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

		void handleTokenInfo(osc::ReceivedMessageArgumentStream args)
		{
			const char* pname;
			int pcount;

			args >> pname >> pcount;

			// Chercher le pool ou est pname.
			auto p = localClient.model().net().findPlace(std::string(pname));
			p->setTokenCount(pcount);
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


		ClientManager remoteClients;
		LocalClient localClient;
};

#endif // CLIENTLOGIC_H
