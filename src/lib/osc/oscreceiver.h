#pragma once

#include <ip/UdpSocket.h>
#include <osc/OscPacketListener.h>
#include <memory>
#include <thread>
#include <functional>
#include <map>
#include <iostream>

class OscReceiver
{
	public:
		using message_handler = std::function<void(osc::ReceivedMessageArgumentStream)>;

		OscReceiver(unsigned int port):
			_port(port)
		{
			bool ok = false;
			while(!ok)
			{
				try
				{
					socket = std::make_shared<UdpListeningReceiveSocket>
							 (IpEndpointName(IpEndpointName::ANY_ADDRESS, _port),
							  &_impl);
					ok = true;
				}
				catch(std::runtime_error& e)
				{
					_port++;
				}
			}
		}

		void run()
		{
			_runThread = std::thread(&UdpListeningReceiveSocket::Run, socket.get());
		}

		template<typename T, class C>
		void addHandler(const std::string &s, T&& theMember, C&& theClass)
		{
			_impl.addHandler(s, std::bind(theMember, theClass, std::placeholders::_1));
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
		std::shared_ptr<UdpListeningReceiveSocket> socket;
		class : public osc::OscPacketListener
		{
			public:
				void addHandler(const std::string& s, const message_handler& h)
				{
					_map[s] = h;
				}

			protected:
				virtual void ProcessMessage(const osc::ReceivedMessage& m,
											const IpEndpointName&) override
				{
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
