#pragma once
#include "clock.h"
#include "oscsender.h"
#include <osc/OscOutboundPacketStream.h>
#include <vector>

class ClockServer
{
	public:
		ClockServer(Clock& clock, OscSender& sender):
			p(buffer.data(), buffer.size()),
			_sender(sender)
		{
			clock.addHandle(std::bind(&ClockServer::send, this, std::placeholders::_1));
		}

		void send(long long int i)
		{
			p.Clear();

			p << osc::BeginBundleImmediate
			  << osc::BeginMessage( "/clock" ) << (osc::int32) i << osc::EndMessage
			  << osc::EndBundle;

			_sender.send(p);
		}

	private:
		std::array<char, 1024> buffer;
		osc::OutboundPacketStream p;

		OscSender& _sender;
};
