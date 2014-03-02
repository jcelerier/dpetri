#pragma once
#include <string>
#include "../nodepool.h"

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

	protected:
		int _id;
		std::string _name;
		const std::string _ip;
		const int _port;

		NodePool _localPool;
};
