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
			nodes.clear();
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

		OwnedNode& operator[](std::string s)
		{
			auto it = std::find_if(nodes.begin(),
								   nodes.end(),
								   [&s] (OwnedNode& n)
			{ return n.node->getName() == s; });

			if(it == nodes.end()) throw "Bad node";

			return *it;
		}

		OwnedNode& operator[](unsigned int i)
		{
			auto it = std::find_if(nodes.begin(),
								   nodes.end(),
								   [i] (OwnedNode& n)
			{ return n.id == i; });

			if(it == nodes.end()) throw "Bad node";

			return *it;
		}

		std::list<OwnedNode> nodes;
};
