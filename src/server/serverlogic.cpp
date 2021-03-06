#include "serverlogic.h"

ServerLogic::ServerLogic(QObject* parent):
	CommonLogic(0, "server", 7000, parent)
{
	localClient.receiver().addHandler("/connect/discover",
									  &ServerLogic::handleConnection, this);
}

void ServerLogic::sendPlay()
{
	// Chercher le client qui a l'état init
	startAlgorithm();

	for(RemoteClient<pnapi::PetriNet>& c : remoteClients)
	{
		c.send("/execution/start");
	}
}

void ServerLogic::handleConnection(osc::ReceivedMessageArgumentStream args)
{
	//// Récupération des données extérieures
	int id; // inutilisé
	const char* hostname;
	const char* ip;
	osc::int32 port;

	args  >> hostname >> id >> ip >> port >> osc::EndMessage;

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
	for(RemoteClient<pnapi::PetriNet>& c : remoteClients)
		newClient.initConnectionTo(c);

    //// Envoi d'un message de ping
    newClient.setPingTimestamp(getTime());
    newClient.send("/ping");
}
