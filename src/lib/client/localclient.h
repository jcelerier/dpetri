#pragma once
#include<string>
#include<fstream>
#include "clock.h"
#include "gui/petrinetmodel.h"
#include "nodepool.h"
#include "client.h"
#include "oscreceiver.h"

class LocalClient : public Client
{
		using SignalHandler = std::function<void()>;
	private:
		PetriNetModel _netModel;
		Clock _clock;
		NodePool _pool;

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
			Client(id, hostname, ip, port),
			_receiver(port),
			_netChanged(netChanged),
			_poolChanged(poolChanged)
		{
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
			_pool.reload(_netModel.net());
			_poolChanged();
		}

		NodePool& pool()
		{
			return _pool;
		}

		const Clock& clock() const
		{
			return _clock;
		}

		PetriNetModel& model()
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
			args >> _id >> osc::EndMessage;
		}

};
