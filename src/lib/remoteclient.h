#pragma once
#include <oscsender.h>
#include <list>
#include <string>
#include <pnapi/pnapi.h>
#include "nodepool.h"
#include "osctools.h"



class RemoteClient
{
	public:
		RemoteClient(const int id,
					 const std::string& hostname,
					 const std::string& ip,
					 const int port):
			_id(id),
			_name(hostname),
			_sender(ip, port)
		{
			// Cas serveur
			if(id != 0)
				send(osc::MessageGenerator()("/connect/id", _id));
		}

		RemoteClient(RemoteClient&& c) = default;
		RemoteClient(const RemoteClient& c) = delete;
		RemoteClient& operator=(const RemoteClient& c) = delete;



		unsigned int id() const
		{
			return _id;
		}

		const std::string& name() const
		{
			return _name;
		}

		void send(const osc::OutboundPacketStream& s)
		{
			_sender.send(s);
		}

		// A appeler si on reçoit un message osc d'un autre client : /pool/take...
		void take(unsigned int nodeId, NodePool& fromPool)
		{
			exchange(nodeId, fromPool, _localPool);
		}

		void give(unsigned int nodeId, NodePool& toPool)
		{
			exchange(nodeId, _localPool, toPool);
		}

		NodePool& pool()
		{
			return _localPool;
		}

	private:
		void exchange(unsigned int nodeId, NodePool& from, NodePool& to)
		{
			// Si node est dans pool local:
			auto it = std::find_if(from.nodes.begin(),
								   from.nodes.end(),
								   [nodeId] (OwnedNode& n)
			{ return n.id == nodeId; });

			if(it == from.nodes.end()) throw;

			to.nodes.splice(to.nodes.end(), from.nodes, it);
		}

		int _id;
		std::string _name;
		OscSender _sender;
		NodePool _localPool;//TODO passer en liste ?
};
