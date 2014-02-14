#pragma once

#include <osc/OscReceivedElements.h>

class ClockClient
{
	public:
		ClockClient(Clock& clock):
			c(clock)
		{

		}

		void clockMessage(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 a2;
			args >> a2 >> osc::EndMessage;

			c.set(a2);
		}

		Clock& c;
};
