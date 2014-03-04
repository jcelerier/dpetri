#pragma once
#include<string>
#include<fstream>
#include "clock/clock.h"
#include "gui/petrinetmodel.h"
#include "client/client.h"
#include "osc/oscreceiver.h"

template<class PetriNetImpl>
class LocalClient : public Client<PetriNetImpl>
{
		using SignalHandler = std::function<void()>;
	private:
		PetriNetModel<PetriNetImpl> _netModel;
		Clock _clock;

		OscReceiver _receiver;

		SignalHandler _netChanged;
		SignalHandler _poolChanged;

	public:
		LocalClient(const int id,
					const std::string& hostname,
					const std::string& ip,
					const int port,
					SignalHandler netChanged,
					SignalHandler poolChanged):
			Client<PetriNetImpl>(id, hostname, ip, -1),
			_receiver(port),
			_netChanged(netChanged),
			_poolChanged(poolChanged)
		{
			this->_port = _receiver.port();
			_receiver.run();
		}

		void loadNet(std::istream& s)
		{
			_netModel.reload(s);
			_netChanged();
		}

		void loadNetAndPool(std::istream& s)
		{
			loadNet(s);
			this->_localPool.reload(_netModel.net());
			_poolChanged();
		}

		Clock& clock()
		{
			return _clock;
		}

		PetriNetModel<PetriNetImpl>& model()
		{
			return _netModel;
		}

		OscReceiver& receiver()
		{
			return _receiver;
		}

		void handlePetriNetReception(osc::ReceivedMessageArgumentStream args)
		{
			osc::Blob b;
			args >> b >> osc::EndMessage;

			std::stringstream s;
			s << static_cast<const char*>(b.data);
			loadNet(s);
		}

		void handleIdReception(osc::ReceivedMessageArgumentStream args)
		{
			args >> this->_id >> osc::EndMessage;
		}
};
