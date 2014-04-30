#pragma once
#include "../osc/oscsender.h"
#include <string>
#include "../osc/oscmessagegenerator.h"
#include "client.h"

template<class PetriNetImpl>
class RemoteClient : public Client<PetriNetImpl>
{
	public:
		RemoteClient(const int id,
					 const std::string& hostname,
					 const std::string& ip,
					 const int port):
			Client<PetriNetImpl>(id, hostname, ip, port),
			_sender(ip, port)
		{
		}

		virtual ~RemoteClient() = default;
		RemoteClient(RemoteClient<PetriNetImpl>&&) = default;
		RemoteClient(const RemoteClient<PetriNetImpl>&) = delete;
		RemoteClient& operator=(const RemoteClient<PetriNetImpl>&) = delete;

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
		void initConnectionTo(Client<PetriNetImpl>& c)
		{
			if(c.id() != this->_id)
			{
				qDebug() << c.ip().c_str() << c.port();
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

