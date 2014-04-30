#pragma once
#include <string>
#include "../nodepool.h"

template<class PetriNetImpl>
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
		Client(Client&&) = default;
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;

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

		void setIp(std::string ip)
		{
			_ip = ip;
		}

		int port() const
		{
			return _port;
		}

		// A appeler si on reçoit un message osc d'un autre client : /pool/take...
		void give(unsigned int nodeId, Client& to)
		{
			to._localPool.take(this->_localPool, nodeId);
		}

		NodePool<PetriNetImpl>& pool()
		{
			return _localPool;
		}

	protected:
		osc::int32 _id;
		std::string _name;
		std::string _ip;
		int _port;

		NodePool<PetriNetImpl> _localPool;
};
