#pragma once
#include <pnapi/pnapi.h>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../clock.h"
#include "../nodepool.h"


using namespace pnapi;
class PetriNetModel : public QObject
{
		Q_OBJECT
	public:
		PetriNet net;
		NodePool pool;
		Clock clock;

		PetriNetModel(QObject * parent):
			QObject(parent)
		{
			//connect(this, SIGNAL(netChanged()), this, SLOT(doLocalChanges()));
		}

	public slots:
		/*
		void doLocalChanges()
		{
			pool.reload(net);
			emit poolChanged();
		}
		*/


		void handleDump(osc::ReceivedMessageArgumentStream args)
		{
			osc::Blob b;
			args >> b >> osc::EndMessage;

			// Charger dans le pool du client 0
			/*
			 * auto it = std::find_if(connectMgr.clients().begin(),
								   connectMgr.clients().end(),
								   [] (RemoteClient& cl)
									{ return cl.id() == 0; });
			*/
			pool.load(net, static_cast<const char*>(b.data));
			emit poolChanged();
		}

		void start()
		{
			clock.start();
			algorithmThread = std::thread(&PetriNetModel::simpleExecutionAlgorithm, this);
		}


		void loadFile()
		{
			QString file = QFileDialog::getOpenFileName(nullptr,
														tr("Choose a PNML file"),
														QString(),
														tr("FIONA File (*.fiona)"));

			std::ifstream in (file.toStdString());
			in >> io::owfn >> net;

			emit netChanged();
			pool.reload(net);
		}

		void loadFromString(std::string str)
		{
			std::stringstream s(str);
			s >> io::owfn >> net;

			emit netChanged();
		}

	signals:
		void netChanged();
		void poolChanged();

	private:
		std::thread algorithmThread;
		void simpleExecutionAlgorithm()
		{
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

							emit netChanged();
							//std::ofstream outfile(std::to_string(i++) + ".dot");
							//outfile << io::dot << net;
						}
					}
				}
			}

			clock.stop();
		}
};
