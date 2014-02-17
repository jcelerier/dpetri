#pragma once
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

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
		}

		virtual void send(const osc::OutboundPacketStream& m) override
		{
			transmitSocket.Send( m.Data(), m.Size() );
		}

	private:
		UdpTransmitSocket transmitSocket;
};
