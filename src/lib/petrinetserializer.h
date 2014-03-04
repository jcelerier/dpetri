#pragma once
#include <string>
#include <sstream>
#include <functional>
#include "type_helper.h"

template<typename PetriNetImpl>
class PetriNetSerializer
{
	public:
		PetriNetSerializer(const PetriNetImpl& net):
			_net(net)
		{
		}

		~PetriNetSerializer()
		{
			delete[] serializedData;
		}

		static void fromSimpleText(const char* text, PetriNetImpl& net)
		{
			std::istringstream s(text);
			std::string line;

			while(std::getline(s, line))
			{
				std::istringstream l(line);

				std::string type;
				l >> type;
				if(type == "place")
				{
					std::string placeName;
					int token;
					l >> placeName >> token;

					net.createPlace(placeName);
					net.setTokenCount(token);
				}
				else if(type == "trans")
				{
					std::string transName;
					int cost;
					l >> transName >> cost;

					auto& t = net.createTransition(transName);
					t.setCost(cost);
				}
				else if(type == "arc")
				{
					std::string orig, dest;
					l >> orig >> dest;
					auto norig = net.findNode(orig);
					auto ndest = net.findNode(dest);
					net.createArc(*norig, *ndest);
				}
			}
		}

		const char* toSimpleText()
		{
			std::stringstream spnet;
			for(place_type* p : _net.getPlaces())
			{
				spnet << "place " << p->getName() << p->getTokenCount() << std::endl;
			}
			for(transition_type* t : _net.getTransitions())
			{
				spnet << "trans " << t->getName() << " " << t->getCost() << std::endl;
			}
			for(arc_type* a : _net.getArcs())
			{
				spnet << "arc " << a->getSourceNode().getName() << " " << a->getTargetNode().getName() << std::endl;
			}

			return toChar(spnet);
		}

		unsigned int size() const
		{
			return _size;
		}

	private:
		const char* toChar(std::stringstream& spnet)
		{
			std::string strpnet = spnet.str();

			delete[] serializedData;
			serializedData = new char[strpnet.size() + 1];
			_size = strpnet.size() + 1;

			std::copy(strpnet.begin(), strpnet.end(), serializedData);
			serializedData[strpnet.length()] = 0;

			return serializedData;
		}

		const PetriNetImpl& _net;
		unsigned int _size = 0;
		char * serializedData = nullptr;
};


#ifdef USE_PNAPI
#include <pnapi/pnapi.h>
#include <graphviz/gvc.h>
template<>
class PetriNetSerializer<pnapi::PetriNet>
{
	public:
		PetriNetSerializer<pnapi::PetriNet>(const pnapi::PetriNet& net):
			_net(net)
		{
		}

		~PetriNetSerializer()
		{
			delete[] serializedData;
		}

		const char* toPNML()
		{
			std::stringstream spnet;
			spnet << pnapi::io::pnml << _net;

			return toChar(spnet);
		}

		const char* toFIONA()
		{
			std::stringstream spnet;
			spnet << pnapi::io::owfn << _net;

			return toChar(spnet);
		}

		const char* toDOT()
		{
			std::stringstream spnet;
			spnet << pnapi::io::dot << _net;

			return toChar(spnet);
		}


		static void fromSimpleText(const char* text, pnapi::PetriNet& net)
		{
			std::istringstream s(text);
			std::string line;

			while(std::getline(s, line))
			{
				std::istringstream l(line);

				std::string type;
				l >> type;
				if(type == "place")
				{
					std::string placeName;
					int token;
					l >> placeName >> token;

					auto& p = net.createPlace(placeName);
					p.setTokenCount(token);
				}
				else if(type == "trans")
				{
					std::string transName;
					int cost;
					l >> transName >> cost;

					auto& t = net.createTransition(transName);
					t.setCost(cost);
				}
				else if(type == "arc")
				{
					std::string orig, dest;
					l >> orig >> dest;
					auto norig = net.findNode(orig);
					auto ndest = net.findNode(dest);
					net.createArc(*norig, *ndest);
				}
			}
		}

		const char* toSimpleText()
		{
			std::stringstream spnet;
			for(place_type* p : _net.getPlaces())
			{
				spnet << "place " << p->getName() << " " << p->getTokenCount() << std::endl;
			}
			for(transition_type* t : _net.getTransitions())
			{
				spnet << "trans " << t->getName() << " " << t->getCost() << std::endl;
			}
			for(arc_type* a : _net.getArcs())
			{
				spnet << "arc " << a->getSourceNode().getName() << " " << a->getTargetNode().getName() << std::endl;
			}

			return toChar(spnet);
		}

		const char* toSVG()
		{
			toDOT();

			GVC_t* gvc = gvContext();
			Agraph_t* G = agmemread(serializedData);
			delete[] serializedData;

			agset(G, (char*) "label", (char*)"");
			gvLayout (gvc, G, "dot");

			gvRenderData(gvc, G, "svg", &serializedData, &_size);
			gvFreeLayout(gvc, G);

			agclose (G);

			gvFreeContext(gvc);

			return serializedData;
		}

		unsigned int size() const
		{
			return _size;
		}

	private:
		const char* toChar(std::stringstream& spnet)
		{
			std::string strpnet = spnet.str();

			delete[] serializedData;
			serializedData = new char[strpnet.size() + 1];
			_size = strpnet.size() + 1;

			std::copy(strpnet.begin(), strpnet.end(), serializedData);
			serializedData[strpnet.length()] = 0;

			return serializedData;
		}

		const pnapi::PetriNet& _net;
		unsigned int _size = 0;
		char * serializedData = nullptr;
};
#endif
