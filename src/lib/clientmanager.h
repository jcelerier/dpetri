#pragma once
#include <vector>
#include <string>
#include "client/remoteclient.h"
#include "nodepool.h"

class ClientManager
{
		friend std::vector<RemoteClient>::iterator begin(ClientManager& c);
		friend std::vector<RemoteClient>::iterator end(ClientManager& c);

	private:
		std::vector<RemoteClient> _clients;
		unsigned int _lastId = 0;

	public:
		ClientManager() = default;
		ClientManager(const ClientManager& c) = delete;
		ClientManager& operator=(const ClientManager& c) = delete;

		RemoteClient& createConnection(const int id,
									   std::string hostname,
									   const std::string& ip,
									   const int port)
		{
			std::cerr << "Création d'un nouveau client" << std::endl;
			// Chercher si un client a déjà le même nom
			while(std::any_of(_clients.begin(),
							  _clients.end(),
							  [&hostname] (RemoteClient& c)
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

		RemoteClient& createConnection(std::string hostname,
									   const std::string& ip,
									   const int port)
		{
			return createConnection((hostname == "server")? 0 : ++_lastId,
									hostname,
									ip,
									port);
		}

		std::vector<RemoteClient>& clients()
		{
			return _clients;
		}

		// Returns client by id
		RemoteClient& operator[](int i)
		{
			auto it = std::find_if(_clients.begin(),
								   _clients.end(),
								   [i] (RemoteClient& cl)
			{ return cl.id() == i; });

			if(it == _clients.end()) throw "Bad client";

			return *it;
		}

		RemoteClient& operator[](std::string& s)
		{
			auto it = std::find_if(_clients.begin(),
								   _clients.end(),
								   [&s] (RemoteClient& cl)
			{ return cl.name() == s; });

			if(it == _clients.end()) throw "Bad client";

			return *it;
		}

		bool hasClient(int i)
		{
			return std::find_if(_clients.begin(),
								_clients.end(),
								[i] (RemoteClient& cl)
		 { return cl.id() == i; }) != _clients.end();
		}
};

inline std::vector<RemoteClient>::iterator begin(ClientManager& c)
{
	return c._clients.begin();
}

inline std::vector<RemoteClient>::iterator end(ClientManager& c)
{
	return c._clients.end();
}
