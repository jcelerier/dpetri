#pragma once
#include <iostream>
#include <pnapi/pnapi.h>
#include "../clock.h"

using namespace pnapi;
class ExecutionAlgorithmBase
{
	protected:
		PetriNet& _net;
		Clock& _clock;

	public:
		ExecutionAlgorithmBase(PetriNet& net, Clock& clock):
			_net(net),
			_clock(clock)
		{
		}

		virtual void start() = 0;
		virtual ~ExecutionAlgorithmBase() = default;
};

class LocalExecutionAlgorithm : public ExecutionAlgorithmBase
{
	public:
		using ExecutionAlgorithmBase::ExecutionAlgorithmBase;
		virtual void start()
		{
			algorithmThread = std::thread(&LocalExecutionAlgorithm::algo, this);
		}

	private:
		std::thread algorithmThread;
		void algo()
		{
			while(_net.findPlace("final")->getTokenCount() == 0)
			{
				for(Transition* t : _net.getTransitions())
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
						if(_clock.get() > t->getCost())
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

						//	emit netChanged();
						}
					}
				}
			}

			_clock.stop();
		}
};
