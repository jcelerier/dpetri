#include "clientlogic.h"
#include "mainwindow.h"

#include <chrono>
std::string getLastTimestampDigits()
{
	auto t = std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			 return t.substr(t.size() - 4);
}

ClientLogic::ClientLogic(MainWindow* parent):
	CommonLogic(-1,
				QHostInfo::localHostName().toStdString() + getLastTimestampDigits(),
				9877,
				parent)
{

	//// Connection
	localClient.receiver().addHandler("/connect/set_id",
									  &LocalClient::handleIdReception, &localClient);
	localClient.receiver().addHandler("/connect/discover",
									  &ClientLogic::handleConnectDiscover, this);

}


void ClientLogic::handleConnectDiscover(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	const char* name;
	const char* ip;
	osc::int32 port;

	args >> name >> id >> ip >> port >> osc::EndMessage;
	qDebug() << name << id << ip << port;

	if(remoteClients.hasClient(id)) return;
	qDebug() << 1;
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
