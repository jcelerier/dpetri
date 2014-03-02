#pragma once
#include <oscsender.h>
#include <list>
#include <string>
#include <pnapi/pnapi.h>
#include "../osctools.h"

#include "client.h"
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
};

