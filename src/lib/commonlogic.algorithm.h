void addToken(std::string name)
{
	auto p = localClient.model().net().findPlace(name);
	if(p)
	{
		p->setTokenCount(p->getTokenCount() + 1);
		emit localNetChanged();
	}

	// chercher dans tous les pools clients
	for(RemoteClient& c : remoteClients)
	{
		c.send("/petrinet/addToken", name.c_str());
	}
}

void removeToken(std::string name)
{
	// Trouver qui possède cette node (ou broadcast ?)
	auto p = localClient.model().net().findPlace(name);
	if(p)
	{
		p->setTokenCount(p->getTokenCount() - 1);
		emit localNetChanged();
	}

	// chercher dans tous les pools clients
	for(RemoteClient& c : remoteClients)
	{
		c.send("/petrinet/removeToken", name.c_str());
	}
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

void checkTransitions(Clock::time_type time)
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
						   [] (Node* n)	{return dynamic_cast<Place*>(n)->getTokenCount() > 0;})
					&& time > t->getCost())
			{
				executeTransition(t);
			}
		}
	}
}
