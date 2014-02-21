#pragma once
#include <vector>
#include <string>
#include "remoteclient.h"
#include "nodepool.h"

class ClientManager
{
	private:
		NodePool& _pool;
		std::vector<RemoteClient> _clients;
		unsigned int _lastId = 0;

	public:
		ClientManager(NodePool& pool):
			_pool(pool)
		{

		}

		RemoteClient& createConnection(const std::string& hostname,
									   const std::string& ip,
									   const int port)
		{
			// Les ID partent de 1 (server = 0)
			_clients.emplace_back((hostname == "server")? 0 : ++_lastId,
								  hostname,
								  ip,
								  port);

			return _clients.back();
		}

		std::vector<RemoteClient>& clients()
		{
			return _clients;
		}

		// Handlers pour take et giveback


};
