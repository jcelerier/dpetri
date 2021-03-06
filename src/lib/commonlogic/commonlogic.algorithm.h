long int getTime()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void startAlgorithm()
{
	createTimeTable();
	localClient.clock().start();
	// Calculer l'heure de démarrage
    _startMs = getTime();

    emit sendLog("Démarrage à : " + QString::number(_startMs));
}

void addToken(std::string name)
{
	auto p = localClient.model().net().findPlace(name);
	if(p)
	{
		p->setTokenCount(p->getTokenCount() + 1);
		emit localNetChanged();
	}

	// chercher dans tous les pools clients
	for(RemoteClient<PetriNetImpl>& c : remoteClients)
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
	for(RemoteClient<PetriNetImpl>& c : remoteClients)
	{
		c.send("/petrinet/removeToken", name.c_str());
	}
}

// Fonction d'exécution de transition.
void executeTransition(transition_type* t, long int origTime)
{
	emit sendLog(QString("Ms écoulées : %2. Commencement de : %1, d'une durée de %3 ms.")
				 .arg(QString::fromStdString(t->getName()))
				 .arg(QString::number(origTime - _startMs))
				 .arg(QString::number(t->getCost())));
	using namespace std::chrono;
	while(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() - origTime <= t->getCost() - 1)
	{
		continue;
	}

	// 1. Vérifier qu'antécédents ont jetons est fait avant
	for(node_type* n : t->getPreset())
	{
		removeToken(n->getName());
	}

	// 2. Envoyer nouveau jeton aux suivants
	for(node_type* n : t->getPostset())
	{
		addToken(n->getName());
	}

    long zeTime = getTime();
	emit sendLog(QString("Ms écoulées : %2. Transition effectuée: %1.")
				 .arg(QString::fromStdString(t->getName()))
				 .arg(QString::number(zeTime - _startMs)));
}

// Quand on reçoit un jeton sur une place, on en informe la machine gérant la transitions suivantes
// pour qu'elles puissent chercher.
// Ou alors les messages de transmission de jeton sont transmis à tout le monde ?
void updateRemotePlace(place_type* place)
{
	auto t = dynamic_cast<transition_type*>(*place->getPostset().begin());

	// Trouver qui possède cette transition (ou broadcast ?)
	if(!localClient.pool().hasNode(t->getName()))
	{
		// chercher dans tous les pools clients
		for(RemoteClient<PetriNetImpl>& c : remoteClients)
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
	for(transition_type* t : localClient.model().net().getTransitions())
	{
		if(localClient.pool().hasNode(t->getName()))
		{
			for(auto& ts : _timeVector)
			{
				if(ts.transition == t && !ts.activated)
				{
					if(std::all_of(t->getPreset().begin(),
								   t->getPreset().end(),
								   [] (node_type* n)	{return dynamic_cast<place_type*>(n)->getTokenCount() > 0;}))
					{
						ts.activated = true;
						using namespace std::chrono;
						ts.runThread = std::thread(&CommonLogic::executeTransition, this, t, duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
					}
				}
			}

		}
	}
}
