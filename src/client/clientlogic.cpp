#include "clientlogic.h"

#include <QNetworkInterface>
#include <QHostInfo>
#include <functional>
#include "mainwindow.h"

QString getIp(QHostAddress serverIP)
{
	// Cas local
	if(serverIP == QHostAddress::LocalHost) return "127.0.0.1";

	// Cas distant
	QList<QHostAddress> list = QNetworkInterface::allAddresses();

	for(int nIter=0; nIter<list.count(); nIter++)
	{
		if(!list[nIter].isLoopback())
			if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
				return list[nIter].toString();
	}

	return "0.0.0.0";
}


ClientLogic::ClientLogic(MainWindow* parent):
	QObject(parent),
	localClient(-1, // Est assigné par serveur
				QHostInfo::localHostName().toStdString(),
				getIp(QHostAddress::LocalHost).toStdString(),
				9877,
				std::bind(&ClientLogic::localNetChanged, this),
				std::bind(&ClientLogic::localPoolChanged, this))
{

	//// Connection
	localClient.receiver().addHandler("/connect/set_id",
						&LocalClient::handleIdReception, &localClient);
	localClient.receiver().addHandler("/connect/discover",
						&ClientLogic::handleConnectDiscover, this);

	//// Réseau de Petri
	localClient.receiver().addHandler("/petrinet/dump",
						&LocalClient::handlePetriNetReception, &localClient);

	//// Pool
	localClient.receiver().addHandler("/pool/dump",
						&ClientLogic::handlePoolDump, this);
	localClient.receiver().addHandler("/pool/ackTake",
						&ClientLogic::handleAckTake, this);
	localClient.receiver().addHandler("/pool/ackGive",
						&ClientLogic::handleAckGive, this);

	//// Jetons
	localClient.receiver().addHandler("/petrinet/addToken",
						&ClientLogic::handleAddToken, this);
	localClient.receiver().addHandler("/petrinet/removeToken",
						&ClientLogic::handleRemoveToken, this);
	localClient.receiver().addHandler("/petrinet/tokenInfo",
						&ClientLogic::handleTokenInfo, this);

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

void ClientLogic::handleAckTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = remoteClients[id];
	origin.give(nodeId, localClient.pool());

	emit serverPoolChanged();
	emit localPoolChanged();

	std::cerr << "Number  of clients: " << remoteClients.clients().size() << std::endl;

	for(RemoteClient& c : remoteClients)
	{
		if(c.id() != 0) // Pas le serveur (why not ?)
			localClient.pool().dumpTo(localClient.id(), c);
	}

	// Potentiellement unifier en rajoutant un message
	// "/pool/confirm id-from id-to id-node"
	// que chacun applique ?
	// -> plus besoin d'envoyer les pool (mais cas ou ils ne communiquent pas ?)
}


void ClientLogic::handleAckGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	auto& origin = remoteClients[id];
	origin.take(nodeId, localClient.pool());

	emit serverPoolChanged();
	emit localPoolChanged();

	for(RemoteClient& c : remoteClients)
	{
		if(c.id() != 0) // Pas le serveur (why not?)
			localClient.pool().dumpTo(localClient.id(), c);
	}
}

void ClientLogic::handlePoolDump(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::Blob b;
	args >> id >> b >> osc::EndMessage;

	std::cerr << "Id réception dump: " << id << std::endl;
	std::cerr << "Nb clients: " << remoteClients.clients().size() << std::endl;
	remoteClients[id].pool().loadFromString(localClient.model().net(),
										static_cast<const char*>(b.data));

	if(id == 0) emit serverPoolChanged(); // Serveur
}


void ClientLogic::takeNode(QString s)
{
	// Faire un take vers le pool du client d'id 0 (le serveur)
	auto& server = remoteClients[0];
	auto& node = server.pool()[s.toStdString()];

	server.send("/pool/take",
				(osc::int32) localClient.id(),
				(osc::int32) node.id);
}

void ClientLogic::giveNode(QString s)
{
	// Faire un give vers le pool du client d'id 0 (le serveur)
	auto& server = remoteClients[0];
	auto& node = localClient.pool()[s.toStdString()];

	server.send("/pool/give",
				(osc::int32) localClient.id(),
				(osc::int32) node.id);
}
