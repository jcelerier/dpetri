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

		int id() const
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
			_localPool.take(fromPool, nodeId);
		}

		void give(unsigned int nodeId, NodePool& toPool)
		{
			toPool.take(_localPool, nodeId);
		}

		NodePool& pool()
		{
			return _localPool;
		}

	private:
		int _id;
		std::string _name;
		OscSender _sender;
		NodePool _localPool;
};
