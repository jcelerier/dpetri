#include <iostream>
#include <thread>
#include <fstream>
#include <pnapi/pnapi.h>
#include <climits>
#include "clock.h"
#include "clockserver.h"
#include "clockclient.h"
#include "oscreceiver.h"

using namespace std;

using namespace pnapi;

// Algorithme :
// Quand "start" : part du premier noeud.
// Règles pour franchir transition :
// - Jetons envoyés
// - Temps écoulé

void test(PetriNet& net, Clock& clock)
{
	int i = 0;
	while(net.findPlace("final")->getTokenCount() == 0)
	{
		for(Transition* t : net.getTransitions())
		{
			// Trouver antécédents
			// Si antécédents ont jeton et temps suffisant
			// Supprimer jetons des antécédents
			// Ajouter jetons aux suivants
			std::vector<std::reference_wrapper<Place>> pre;
			unsigned int count{0};
			// Tests sur antécédents
			for(Arc* a : t->getPresetArcs())
			{
				pre.push_back(a->getPlace());
			}

			for(Place& place : pre)
			{
				// Trouver le nombre minimum de jetons communs
				if(place.getTokenCount() > 0)
					count++;
			}

			if(count == t->getPresetArcs().size()) // Trouver le nombre qu'on peut enlever
			{
				unsigned int min{INT_MAX};
				for(Place& place : pre)
				{
					min = std::min(place.getTokenCount(), min);
				}

				// Application sur suivants
				if(clock.get() > t->getCost())
				{
					for(Place& place : pre)
					{
						place.setTokenCount(place.getTokenCount() - min);
					}

					std::cerr << "nouvel état!" << std::endl;
					for(Arc* a : t->getPostsetArcs())
					{
						auto& place = a->getPlace();
						place.setTokenCount(place.getTokenCount() + 1);
					}

					std::ofstream outfile(std::to_string(i++) + ".dot");
					outfile << io::dot << net;
				}
			}
		}
	}

	clock.stop();
}

void clientTest()
{
	OscReceiver r;
	Clock c;
	ClockClient h(c);
	auto f = std::bind(&ClockClient::clockMessage, &h, std::placeholders::_1);
	r.addHandler("/clock", f);
	r.run();
}

void serverTest()
{
	Clock c(500);
	ClockServer s(c);
	c.run();
}

int main()
{
	clientTest();
	return 0;
}

