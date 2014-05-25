#pragma once
#include <list>
#include <sstream>

#include <osc/OscReceivedElements.h>
#include "osc/oscmessagegenerator.h"
#include "ownednode.h"

#include "type_helper.h"
template<class PetriNetImpl>
class NodePool
{
		template<class T>
		friend typename std::list<OwnedNode>::iterator begin(NodePool<T>& p);
		template<class T>
		friend typename std::list<OwnedNode>::iterator end(NodePool<T> &p);

	public:
		NodePool() = default;
		NodePool(NodePool<PetriNetImpl>&&) = default;
		NodePool(const NodePool<PetriNetImpl>&) = delete;
		NodePool& operator=(const NodePool<PetriNetImpl>&) = delete;

		void reload(const PetriNetImpl& net) // A appeler sur serveur
		{
			unsigned int id = 0;
			auto pn_nodes = net.getNodes();
			for(node_type* node : pn_nodes)
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

		void loadFromString(const PetriNetImpl& net, const char* str)
		{
            std::cerr << str;
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
			return std::find_if(std::begin(_nodes),
								std::end(_nodes),
								[&s] (OwnedNode& n)
		 { return n.node->getName() == s; }) != std::end(_nodes);
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

		void take(NodePool<PetriNetImpl>& from, unsigned int nodeId)
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
			//TODO Cache
			auto str2 = dump();
			c.send(osc::MessageGenerator(1024 + str2.size(),
										 "/pool/dump",
										 localid,
										 osc::Blob(str2.c_str(), str2.size())));
		}

		int size() const
		{
			return _nodes.size();
		}

	private:
		std::list<OwnedNode> _nodes;
};

template<class PetriNetImpl>
typename std::list<OwnedNode>::iterator begin(NodePool<PetriNetImpl>& p)
{
	return p._nodes.begin();
}

template<class PetriNetImpl>
typename std::list<OwnedNode>::iterator end(NodePool<PetriNetImpl>& p)
{
	return p._nodes.end();
}

