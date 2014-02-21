#pragma once

#include <ip/UdpSocket.h>
#include <osc/OscPacketListener.h>
#include <thread>
#include <functional>
#include <map>
#include <iostream>

class OscReceiver
{
	public:
		using message_handler = std::function<void(osc::ReceivedMessageArgumentStream)>;

		OscReceiver(unsigned int port):
			_port(port),
			socket(IpEndpointName(IpEndpointName::ANY_ADDRESS, port), &_impl)
		{
		}

		void run()
		{
			_runThread = std::thread(&UdpListeningReceiveSocket::Run, &socket);
		}

		void addHandler(const std::string &s, const	message_handler h)
		{
			_impl.addHandler(s, h);
		}

		unsigned int port() const
		{
			return _port;
		}

	private:
		unsigned int _port = 0;
		UdpListeningReceiveSocket socket;
		class : public osc::OscPacketListener
		{
			public:
				void addHandler(const std::string& s, const message_handler& h)
				{
					_map[s] = h;
				}

			protected:
				virtual void ProcessMessage(const osc::ReceivedMessage& m,
											const IpEndpointName& name) override
				{
					std::cerr << "Message received from: " << name.address << std::endl;
					try
					{
						auto addr = std::string(m.AddressPattern());

						if(_map.find(addr) != _map.end())
							_map[addr](m.ArgumentStream());
					}
					catch( osc::Exception& e )
					{
						std::cerr << "error while parsing message: "
								  << m.AddressPattern() << ": " << e.what() << std::endl;
					}
				}

			private:
				std::map<std::string, message_handler> _map;
		} _impl{};

		std::thread _runThread;
};
