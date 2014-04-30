#include "androidclientlogic.h"
#include "mainwindow.h"
#include <chrono>
#include <string>
std::string getLastTimestampDigits()
{
	std::stringstream s;
	s << std::chrono::high_resolution_clock::now().time_since_epoch().count();
	return s.str().substr(s.str().size() - 4);
}

AndroidClientLogic::AndroidClientLogic(MainWindow* parent):
	CommonLogic<SimplePetriNet>(-1,
				QHostInfo::localHostName().toStdString() + getLastTimestampDigits(),
				9877,
				parent)
{

	//// Connection
	localClient.receiver().addHandler("/connect/set_id",
									  &LocalClient<SimplePetriNet>::handleIdReception, &localClient);
	localClient.receiver().addHandler("/connect/discover",
									  &AndroidClientLogic::handleConnectDiscover, this);

}

void AndroidClientLogic::setLocalAddress(QHostAddress ip)
{
	localClient.setIp(ip.toString().toStdString());
}

void AndroidClientLogic::handleConnectDiscover(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	const char* name;
	const char* ip;
	osc::int32 port;

	args >> name >> id >> ip >> port >> osc::EndMessage;

	if(remoteClients.hasClient(id)) return;
	auto& remote = remoteClients.createConnection(id,
												  std::string(name),
												  std::string(ip),
												  port);

	//// Lui faire la demande de connection
	remote.initConnectionTo(localClient);

	//TODO attention si le réseau lag ici ? faire une attente ?
	//// Lui envoyer notre pool
	localClient.pool().dumpTo(localClient.id(), remote);
}
