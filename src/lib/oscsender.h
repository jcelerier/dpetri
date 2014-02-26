#pragma once
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

#include <string>
#include <iostream>

class OscSenderInterface
{
	public:
		virtual void send(const osc::OutboundPacketStream&) = 0;
		virtual ~OscSenderInterface() = default;
};

// Faire simple et multicast
class OscSender: public OscSenderInterface
{
	public:
		OscSender(const std::string& ip, const int port):
			transmitSocket{std::make_shared<UdpTransmitSocket>(IpEndpointName(ip.c_str(), port))}
		{
			std::cerr << "Connecting to : " << ip << ":" << port << std::endl;
		}

		virtual ~OscSender() = default;
		OscSender(OscSender&&) = default;

		OscSender(const OscSender&) = delete;
		OscSender& operator=(const OscSender&) = delete;

		virtual void send(const osc::OutboundPacketStream& m) override
		{
			transmitSocket->Send( m.Data(), m.Size() );
			std::cerr << "Message correctly sent. Size: " << m.Size() << std::endl ;
		}

	private:
		std::shared_ptr<UdpTransmitSocket> transmitSocket;
};
