#pragma once

#include <pnapi/pnapi.h>
#include <list>
#include <sstream>

#include <osc/OscReceivedElements.h>
#include "ownednode.h"

using namespace pnapi;
class NodePool
{
	public:
		void reload(PetriNet& net) // A appeler sur serveur
		{
			unsigned int id = 0;
			auto pn_nodes = net.getNodes();
			for(Node* node : pn_nodes)
			{
				nodes.emplace_back(node, id++);
			}
		}

		/// Format :
		/// id nomNode\n
		std::string dump()
		{
			std::stringstream s;
			for(auto& node : nodes)
				s << node.id << " " << node.node->getName() << std::endl;

			return s.str();
		}

		void load(PetriNet& net, const char* str)
		{
			std::istringstream s(str);
			std::string line;
			while(std::getline(s, line))
			{
				std::istringstream l(line);

				int id;
				std::string name;
				l >> id >> name;

				//TODO check
				nodes.emplace_back(net.findNode(name), id);
			}
		}

		std::list<OwnedNode> nodes;
};
