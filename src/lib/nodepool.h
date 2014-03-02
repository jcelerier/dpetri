#pragma once

#include <pnapi/pnapi.h>
#include <list>
#include <sstream>

#include <osc/OscReceivedElements.h>
#include "osctools.h"
#include "ownednode.h"
using namespace pnapi;
class NodePool
{
		friend std::list<OwnedNode>::iterator begin(NodePool& p);
		friend std::list<OwnedNode>::iterator end(NodePool &p);

	public:
		NodePool() = default;
		NodePool(NodePool&&) = default;
		NodePool(const NodePool&) = delete;
		NodePool& operator=(const NodePool&) = delete;

		void reload(const PetriNet& net) // A appeler sur serveur
		{
			unsigned int id = 0;
			auto pn_nodes = net.getNodes();
			for(Node* node : pn_nodes)
			{
				_nodes.emplace_back(node, id++);
			}
		}

		/// Format :
		/// id nomNode\n
		std::string dump() const
		{
			std::stringstream s;
			for(auto& node : _nodes)
				s << node.id << " " << node.node->getName() << std::endl;

			return s.str();
		}

		void loadFromString(const PetriNet& net, const char* str)
		{
			_nodes.clear();
			std::istringstream s(str);
			std::string line;

			while(std::getline(s, line))
			{
				std::istringstream l(line);

				int id;
				std::string name;
				l >> id >> name;

				//TODO check
				_nodes.emplace_back(net.findNode(name), id);
			}
		}

		bool hasNode(std::string s)
		{
			return std::find_if(_nodes.begin(),
								_nodes.end(),
								[&s] (OwnedNode& n)
		 { return n.node->getName() == s; }) != _nodes.end();
		}

		OwnedNode& operator[](std::string s)
		{
			auto it = std::find_if(_nodes.begin(),
								   _nodes.end(),
								   [&s] (OwnedNode& n)
			{ return n.node->getName() == s; });

			if(it == _nodes.end()) throw "Bad node";

			return *it;
		}

		OwnedNode& operator[](unsigned int i)
		{
			auto it = std::find_if(_nodes.begin(),
								   _nodes.end(),
								   [i] (OwnedNode& n)
			{ return n.id == i; });

			if(it == _nodes.end()) throw "Bad node";

			return *it;
		}

		void take(NodePool& from, unsigned int nodeId)
		{
			auto it = std::find_if(begin(from),
								   end(from),
								   [nodeId] (OwnedNode& n)
			{ return n.id == nodeId; });

			if(it == end(from)) return;

			_nodes.splice(_nodes.end(), from._nodes, it);
		}

		template<class TClient>
		void dumpTo(int localid, TClient& c)
		{
			std::cerr << "Sending pool dump to: " << c.id() << " " << c.name().c_str() << std::endl;
			//TODO Cache
			auto str2 = dump();
			c.send(osc::MessageGenerator(1024 + str2.size(),
										 "/pool/dump",
										 localid,
										 osc::Blob(str2.c_str(), str2.size())));
		}

	private:
		std::list<OwnedNode> _nodes;
};

inline std::list<OwnedNode>::iterator begin(NodePool& p)
{
	return p._nodes.begin();
}

inline std::list<OwnedNode>::iterator end(NodePool& p)
{
	return p._nodes.end();
}
