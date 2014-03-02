#pragma once
#include <pnapi/pnapi.h>
#include "../petrinettools.h"
#include "../client/remoteclient.h"

using namespace pnapi;
class PetriNetModel
{
	private:
		PetriNet _net;

	public:
		const PetriNet& net() const
		{
			return _net;
		}

		void dumpTo(RemoteClient& c)
		{
			PetriNetSerializer ser(_net);
			const char* dmp = ser.toFIONA();

			c.send(osc::MessageGenerator(1024 + ser.size(),
										 "/petrinet/dump",
										 osc::Blob(dmp, ser.size())));
		}

		void reload(std::istream& s)
		{
			s >> io::owfn >> _net;
		}
};
