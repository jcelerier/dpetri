#pragma once
#include <pnapi/pnapi.h>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../clock.h"


using namespace pnapi;
class PetriNetModel : public QObject
{
		Q_OBJECT
	public:
		PetriNet net;
		Clock clock;

		PetriNetModel(QObject * parent):
			QObject(parent)
		{

		}

	public slots:
		void start()
		{
			clock.run();
			algorithmThread = std::thread(&PetriNetModel::simpleExecutionAlgorithm, this);
		}


		void loadFile()
		{
			QString file = QFileDialog::getOpenFileName(nullptr,
														tr("Choose a PNML file"),
														QString(),
														tr("PNML File (*.pnml)"));

			std::ifstream in (file.toStdString());
			in >> io::pnml >> net;

			emit changed();
		}

		void loadFromString(std::string str)
		{
			std::stringstream s(str);
			s >> io::pnml >> net;

			emit changed();
		}

	signals:
		void changed();

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

							emit changed();
							//std::ofstream outfile(std::to_string(i++) + ".dot");
							//outfile << io::dot << net;
						}
					}
				}
			}

			clock.stop();
		}
};
