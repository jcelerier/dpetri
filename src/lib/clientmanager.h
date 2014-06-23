#pragma once
#include <vector>
#include <string>
#include "client/remoteclient.h"
#include "nodepool.h"

template<class PetriNetImpl>
class ClientManager
{
		template<class T>
		friend typename std::vector<RemoteClient<T>>::iterator begin(ClientManager<T>& c);
		template<class T>
		friend typename std::vector<RemoteClient<T>>::iterator end(ClientManager<T>& c);

	private:
		std::vector<RemoteClient<PetriNetImpl>> _clients;
		unsigned int _lastId = 0;

	public:
		ClientManager() = default;
		ClientManager(const ClientManager<PetriNetImpl>& c) = delete;
		ClientManager& operator=(const ClientManager<PetriNetImpl>& c) = delete;

		RemoteClient<PetriNetImpl>& createConnection(const int id,
									   std::string hostname,
									   const std::string& ip,
									   const int port)
		{
			// Chercher si un client a déjà le même nom
			while(std::any_of(_clients.begin(),
							  _clients.end(),
							  [&hostname] (RemoteClient<PetriNetImpl>& c)
				{ return hostname == c.name();}))
			{
				hostname.append("X");
			}

			// Les ID partent de 1 (server = 0)
			_clients.emplace_back(id,
								  hostname,
								  ip,
								  port);

			return _clients.back();
		}

		/**
		 * @brief createConnection
		 * @param hostname
		 * @param ip
		 * @param port
		 *
		 * @return
		 */
		RemoteClient<PetriNetImpl>& createConnection(std::string hostname,
									   const std::string& ip,
									   const int port)
		{
			return createConnection((hostname == "server")? 0 : ++_lastId,
									hostname,
									ip,
									port);
		}

		std::vector<RemoteClient<PetriNetImpl>>& clients()
		{
			return _clients;
		}

		// Returns client by id
		RemoteClient<PetriNetImpl>& operator[](int i)
		{
			auto it = std::find_if(_clients.begin(),
								   _clients.end(),
								   [i] (RemoteClient<PetriNetImpl>& cl)
			{ return cl.id() == i; });

			if(it == _clients.end()) throw "Bad client";

			return *it;
		}

		RemoteClient<PetriNetImpl>& operator[](std::string& s)
		{
			auto it = std::find_if(_clients.begin(),
								   _clients.end(),
								   [&s] (RemoteClient<PetriNetImpl>& cl)
			{ return cl.name() == s; });

			if(it == _clients.end()) throw "Bad client";

			return *it;
		}

		bool hasClient(int i)
		{
			return std::find_if(_clients.begin(),
								_clients.end(),
								[i] (RemoteClient<PetriNetImpl>& cl)
		 { return cl.id() == i; }) != _clients.end();
		}
};

template<class PetriNetImpl>
typename std::vector<RemoteClient<PetriNetImpl>>::iterator begin(ClientManager<PetriNetImpl>& c)
{
	return c._clients.begin();
}

template<class PetriNetImpl>
typename std::vector<RemoteClient<PetriNetImpl>>::iterator end(ClientManager<PetriNetImpl>& c)
{
	return c._clients.end();
}
