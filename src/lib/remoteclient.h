#pragma once
#include <oscsender.h>
#include <list>
#include <string>
#include <pnapi/pnapi.h>
#include "nodepool.h"
#include "osctools.h"

class Client
{
	public:
		Client(const int id,
			   const std::string& hostname,
			   const std::string& ip,
			   const int port):
			_id(id),
			_name(hostname),
			_ip(ip),
			_port(port)
		{
		}

		virtual ~Client() = default;
		Client(Client&& c) = default;
		Client(const Client& c) = delete;
		Client& operator=(const Client& c) = delete;

		void setId(int id)
		{
			_id = id;
		}

		int id() const
		{
			return _id;
		}

		const std::string& name() const
		{
			return _name;
		}

		const std::string& ip() const
		{
			return _ip;
		}

		int port() const
		{
			return _port;
		}


	protected:
		int _id;
		const std::string _name;
		const std::string _ip;
		const int _port;
};

class RemoteClient : public Client
{
	public:
		RemoteClient(const int id,
					 const std::string& hostname,
					 const std::string& ip,
					 const int port):
			Client(id, hostname, ip, port),
			_sender(ip, port)
		{
		}

		virtual ~RemoteClient() = default;
		RemoteClient(RemoteClient&& c) = default;
		RemoteClient(const RemoteClient& c) = delete;
		RemoteClient& operator=(const RemoteClient& c) = delete;

		// Emission de données vers ce client
		void send(const osc::OutboundPacketStream& s)
		{
			_sender.send(s);
		}

		void send(const osc::MessageGenerator& m)
		{
			_sender.send(m.stream());
		}

		template<typename... T>
		void send(const std::string& name, const T... args)
		{
			send(osc::MessageGenerator(name, args...));
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

		// Cette méthode est appelée par le serveur.
		// Elle dit au client A (this) d'initier la connection avec le client B (c).
		void initConnectionTo(Client& c)
		{
			if(c.id() != _id)
			{
				send("/connect/discover",
					 c.name().c_str(),
					 c.id(),
					 c.ip().c_str(),
					 c.port());
			}
		}

	private:
		OscSender _sender;
		NodePool _localPool;
};

