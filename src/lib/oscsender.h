#pragma once
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

#include <string>
#include <iostream>

class OscSenderInterface
{
	public:
		virtual void send(const osc::OutboundPacketStream&) = 0;
};

// Faire simple et multicast
class OscSender: public OscSenderInterface
{
	public:
		OscSender(const std::string& ip, const int port):
			transmitSocket{IpEndpointName(ip.c_str(), port)}
		{
			std::cerr << "Connecting to : " << ip << ":" << port << std::endl;
		}

		virtual void send(const osc::OutboundPacketStream& m) override
		{
			transmitSocket.Send( m.Data(), m.Size() );
		}

	private:
		UdpTransmitSocket transmitSocket;
};
