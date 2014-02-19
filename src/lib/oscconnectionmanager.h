#pragma once
#include <oscsender.h>
#include <map>
#include <string>

class OSCConnectionManager
{
	private:
		std::map<std::string, OscSender> _senders;

	public:

		OSCConnectionManager() = default;

		OscSender& createConnection(std::string hostname, std::string ip,  int port)
		{
			_senders.emplace(std::piecewise_construct,
							 std::forward_as_tuple(hostname),
							 std::forward_as_tuple(ip, port));
			std::cerr << "Hostname: " << hostname << std::endl;
			return _senders.at(hostname);
		}

		const std::map<std::string, OscSender>& getMap() const
		{
			return _senders;
		}
};
