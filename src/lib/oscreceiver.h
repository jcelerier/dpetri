#pragma once

#include <ip/UdpSocket.h>
#include <osc/OscPacketListener.h>
#include <functional>
#include <map>



class OscReceiver
{
	public:
		using message_handler = std::function<void(osc::ReceivedMessageArgumentStream)>;

		OscReceiver():
			socket(IpEndpointName( IpEndpointName::ANY_ADDRESS, 9876),
			  &_impl)
		{

		}
		void run()
		{
			socket.Run();
		}

		void addHandler(const std::string &s, const	message_handler h)
		{
			_impl.addHandler(s, h);
		}

	private:
		UdpListeningReceiveSocket socket;
		class : public osc::OscPacketListener
		{
			public:

				void addHandler(const std::string& s, const message_handler& h)
				{
					_map[s] = h;
				}

			protected:
				virtual void ProcessMessage( const osc::ReceivedMessage& m,
											 const IpEndpointName&  )
				{
					try
					{
						auto addr = std::string(m.AddressPattern());
						if(_map.find(addr) != _map.end())
						{
							_map[addr](m.ArgumentStream());
						}
					}
					catch( osc::Exception& e )
					{
						std::cerr << "error while parsing message: "
								  << m.AddressPattern() << ": " << e.what() << "\n";
					}
				}

			private:
				std::map<std::string, message_handler> _map;
		} _impl{};
};
