#pragma once
#include <pnapi/pnapi.h>
#include <string>
#include <sstream>
#include <functional>

#include <graphviz/types.h>
#include <graphviz/graph.h>
#include <graphviz/cdt.h>
#include <graphviz/gvc.h>

using namespace pnapi;
class PetriNetSerializer
{
	public:
		PetriNetSerializer(const PetriNet& net):
			_net(net)
		{
		}

		~PetriNetSerializer()
		{
			if(serializedData) delete[] serializedData;
		}

		const char* toPNML()
		{
			std::stringstream spnet;
			spnet << io::pnml << _net;

			return toChar(spnet);

		}

		const char* toDOT()
		{
			std::stringstream spnet;
			spnet << io::dot << _net;

			return toChar(spnet);
		}

		const char* toSVG()
		{
			toDOT();

			GVC_t* gvc = gvContext();
			Agraph_t* G = agmemread(serializedData);
			delete[] serializedData;

			agset(G, (char*) "label", (char*)"");
			gvLayout (gvc, G, "neato");

			gvRenderData(gvc, G, "svg", &serializedData, &_size);
			gvFreeLayout(gvc, G);

			agclose (G);

			gvFreeContext(gvc);

			return serializedData;
		}

		unsigned int size()
		{
			return _size;
		}

	private:
		const char* toChar(std::stringstream& spnet)
		{
			std::string strpnet = spnet.str();

			if(serializedData) delete[] serializedData;
			serializedData = new char[strpnet.size() + 1];
			_size = strpnet.size() + 1;

			std::copy(strpnet.begin(), strpnet.end(), serializedData);
			serializedData[strpnet.length()] = 0;

			return serializedData;
		}

		const PetriNet& _net;
		unsigned int _size = 0;
		char * serializedData = nullptr;
};
