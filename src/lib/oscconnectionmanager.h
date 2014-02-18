#pragma once
#include <oscsender.h>
#include <vector>
#include <string>

class OSCConnectionManager
{
	public:
		OSCConnectionManager() = default;

		OscSender& createConnection(std::string ip,  int port)
		{
			_senders.emplace_back(ip, port);
			return *(_senders.end() - 1);
		}

	private:
		std::vector<OscSender> _senders;
};
