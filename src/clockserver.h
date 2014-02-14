#pragma once
#include "clock.h"
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>
#include <vector>

class ClockServer
{
	public:
		ClockServer(Clock& clock):
			p(buffer.data(), buffer.size())
		{
			clock.addHandle(std::bind(&ClockServer::send, this, std::placeholders::_1));
		}


		void send(long long int i)
		{
			p.Clear();

			p << osc::BeginBundleImmediate
			  << osc::BeginMessage( "/clock" ) << (osc::int32)i << osc::EndMessage
			  << osc::EndBundle;

			transmitSocket.Send( p.Data(), p.Size() );
		}

	private:
		UdpTransmitSocket transmitSocket{IpEndpointName( "127.0.0.1", 9876 ) };
		std::array<char, 1024> buffer;
		osc::OutboundPacketStream p;
};
