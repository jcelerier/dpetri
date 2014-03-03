#pragma once
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

#include <string>
#include <iostream>
#include <memory>

class OscSenderInterface
{
	public:
		virtual void send(const osc::OutboundPacketStream&) = 0;
		virtual ~OscSenderInterface() = default;
};

// Faire simple et multicast,
class OscSender: public OscSenderInterface
{
	public:
		OscSender(const std::string& ip, const int port):
			transmitSocket{std::make_shared<UdpTransmitSocket>(IpEndpointName(ip.c_str(), port))}
		{
		}

		virtual ~OscSender() = default;
		OscSender(OscSender&&) = default;
		OscSender(const OscSender&) = delete;
		OscSender& operator=(const OscSender&) = delete;

		virtual void send(const osc::OutboundPacketStream& m) override
		{
			transmitSocket->Send( m.Data(), m.Size() );
		}

	private:
		std::shared_ptr<UdpTransmitSocket> transmitSocket;
};
