#pragma once
#include <pnapi/pnapi.h>
#include "../petrinetserializer.h"
#include "../client/remoteclient.h"

using namespace pnapi;
template<typename PetriNetImpl>
class PetriNetModel
{
	private:
		PetriNetImpl _net;

	public:
		const PetriNetImpl& net() const
		{
			return _net;
		}

		void dumpTo(RemoteClient<PetriNetImpl>& c)
		{
			PetriNetSerializer<PetriNetImpl> ser(_net);
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
