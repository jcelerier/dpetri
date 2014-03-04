#pragma once
#include "../petrinetserializer.h"
#include "../client/remoteclient.h"
#include <iostream>

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
			const char* dmp = ser.toSimpleText();

			c.send(osc::MessageGenerator(1024 + ser.size(),
										 "/petrinet/dump",
										 osc::Blob(dmp, ser.size())));
		}

		void reload(const char * s)
		{
			PetriNetSerializer<PetriNetImpl>::fromSimpleText(s, _net);
		}
};

#ifdef USE_PNAPI
template<>
class PetriNetModel<pnapi::PetriNet>
{
	private:
		pnapi::PetriNet _net;

	public:
		const pnapi::PetriNet& net() const
		{
			return _net;
		}

		void dumpTo(RemoteClient<pnapi::PetriNet>& c)
		{
			PetriNetSerializer<pnapi::PetriNet> ser(_net);
			const char* dmp = ser.toSimpleText();
			std::cerr << "Sent this: " << dmp;

			c.send(osc::MessageGenerator(1024 + ser.size(),
										 "/petrinet/dump",
										 osc::Blob(dmp, ser.size())));
		}

		void reload(const char * s)
		{
			PetriNetSerializer<pnapi::PetriNet>::fromSimpleText(s, _net);
		}

		void reloadFromFIONA(std::istream& s)
		{
			s >> pnapi::io::owfn >> _net;
		}
};
#endif
