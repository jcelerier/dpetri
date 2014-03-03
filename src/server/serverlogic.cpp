#include "serverlogic.h"

ServerLogic::ServerLogic(QObject* parent):
	CommonLogic(0, "server", 7000, parent)
{
	localClient.receiver().addHandler("/connect/discover",
									  &ServerLogic::handleConnection, this);
}


void ServerLogic::handleConnection(osc::ReceivedMessageArgumentStream args)
{
	//// Récupération des données extérieures
	int id; // inutilisé
	const char* hostname;
	const char* ip;
	osc::int32 port;

	args  >> hostname >> id >> ip >> port >> osc::EndMessage;
	qDebug() << "Handle Connection: " << hostname << id << ip << port;

	//// Création de l'émetteur vers le client
	auto& newClient = remoteClients.createConnection(std::string(hostname),
												 std::string(ip),
												 port);

	newClient.send("/connect/set_id",
				   newClient.id());

	emit connectionListChanged();

	//// Envoi du réseau de petri vers le nouveau client
	localClient.model().dumpTo(newClient);

	//// Envoi du pool vers le nouveau client
	localClient.pool().dumpTo(0, newClient);

	//// Envoi des informations des autres clients vers le nouveau client
	for(RemoteClient& c : remoteClients)
		newClient.initConnectionTo(c);
}
