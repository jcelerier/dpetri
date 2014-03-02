#pragma once
#include <iostream>
#include <pnapi/pnapi.h>
#include "../nodepool.h"
#include "../clock.h"

using namespace pnapi;
class ExecutionAlgorithmBase
{
	public:
		using ChangeCallback = std::function<void()>;
		using TransitionCallback = std::function<void()>;

		ExecutionAlgorithmBase(ChangeCallback cb, PetriNet& net, Clock& clock):
			_net(net),
			_clock(clock),
			_callback(cb)
		{
		}

		virtual void start() = 0;
		virtual ~ExecutionAlgorithmBase() = default;

	protected:
		PetriNet& _net;
		Clock& _clock;
		ChangeCallback _callback;
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

							_callback();
						}
					}
				}
			}

			_clock.stop();
		}
};


class NetworkExecutionAlgorithm : public ExecutionAlgorithmBase
{
	private:
		NodePool& _pool;
	public:
		NetworkExecutionAlgorithm(NodePool& p, ChangeCallback cb, PetriNet& net, Clock& clock):
			ExecutionAlgorithmBase(cb, net, clock),
			_pool(p)
		{
		}


};
