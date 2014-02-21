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
			osc::MessageGenerator m;
			send(m("/connect/id", _id));
		}

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
		void take(unsigned int nodeId, NodePool& pool)
		{
			// Si node est dans pool global:
			// enlever du pool global et mettre dans pool client.
			auto it = std::find_if(pool.nodes.begin(),
								   pool.nodes.end(),
								   [nodeId] (OwnedNode& n)
										{ return n.id == nodeId; });

			if(it != pool.nodes.end())
			{
				pool.nodes.splice(it, _pool.nodes);
				osc::MessageGenerator m;
				send(m("/pool/ackTake", (osc::int32) nodeId));

				//TODO Envoyer un message d'update de pool à tout le monde ?
			}
		}

		void give(unsigned int nodeId, NodePool& pool)
		{
			// Si node est dans pool local:
			auto it = std::find_if(_pool.nodes.begin(), _pool.nodes.end(),
						 [nodeId] (OwnedNode& n) { return n.id == nodeId; });

			if(it != _pool.nodes.end())
			{
				_pool.nodes.splice(it, pool.nodes);
				osc::MessageGenerator m;
				send(m("/pool/ackTake", (osc::int32) nodeId));

				//TODO Envoyer un message d'update de pool à tout le monde ?
			}
		}

		NodePool& pool()
		{
			return _pool;
		}

	private:
		int _id;
		std::string _name;
		OscSender _sender;
		NodePool _pool;//TODO passer en liste ?
};
