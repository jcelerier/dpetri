void handleStart(osc::ReceivedMessageArgumentStream)
{
	startAlgorithm();
}

void handleInfoTransfer(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 nodeId, fromId, toId;
	args >> nodeId >> fromId >> toId;

	remoteClients[fromId].give(nodeId, remoteClients[toId]);

	if(fromId == 0 || toId == 0)
	{
		emit serverPoolChanged();
	}
}

void handleTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 idRemote;
	osc::int32 idNode;
	args >> idRemote >> idNode >> osc::EndMessage;

	auto& rclient = remoteClients[idRemote];
	localClient.give(idNode, rclient);
	rclient.send("/pool/ackTake",
				 0,
				 (osc::int32) idNode); // Cas serveur

	emit localPoolChanged();
	emit clientPoolChanged(rclient.id());
}

void handleGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 idRemote;
	osc::int32 idNode;
	args >> idRemote >> idNode >> osc::EndMessage;

	auto& rclient = remoteClients[idRemote];
	rclient.give(idNode, localClient);
	rclient.send("/pool/ackGive",
				 0,
				 (osc::int32) idNode);

	emit localPoolChanged();
	emit clientPoolChanged(rclient.id());
}


// /pool/infoTransfer node from to;
void handleAckTake(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	remoteClients[id].give(nodeId, localClient);

	if(id == 0) emit serverPoolChanged();
	emit localPoolChanged();

	for(RemoteClient<PetriNetImpl>& c : remoteClients)
	{
		if(c.id() != 0) // Pas le serveur (why not ?)
			c.send("/pool/infoTransfer", nodeId, id, localClient.id());
		//localClient.pool().dumpTo(localClient.id(), c);
	}
}


void handleAckGive(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::int32 nodeId;

	args >> id >> nodeId >> osc::EndMessage;

	localClient.give(nodeId, remoteClients[id]);

	if(id == 0) emit serverPoolChanged();
	emit localPoolChanged();

	for(RemoteClient<PetriNetImpl>& c : remoteClients)
	{
		if(c.id() != 0) // Pas le serveur (il le fait, changer?)
			c.send("/pool/infoTransfer", nodeId, localClient.id(), id);
		//localClient.pool().dumpTo(localClient.id(), c);
	}
}

void handlePoolDump(osc::ReceivedMessageArgumentStream args)
{
	osc::int32 id;
	osc::Blob b;
	args >> id >> b >> osc::EndMessage;

	remoteClients[id].pool().loadFromString(localClient.model().net(),
											static_cast<const char*>(b.data));

	if(id == 0) emit serverPoolChanged(); // Serveur
}


void handleAddToken(osc::ReceivedMessageArgumentStream args)
{
	const char * pname;
	args >> pname >> osc::EndMessage;

	auto place = localClient.model().net().findPlace(pname);
	place->setTokenCount(place->getTokenCount()+1);

	emit localNetChanged();
}

void handleRemoveToken(osc::ReceivedMessageArgumentStream args)
{
	const char * pname;
	args >> pname >> osc::EndMessage;

	auto place = localClient.model().net().findPlace(pname);
	place->setTokenCount(place->getTokenCount() - 1);

	emit localNetChanged();
}

void handleTokenInfo(osc::ReceivedMessageArgumentStream args)
{
	const char* pname;
	osc::int32 pcount;

	args >> pname >> pcount;

	// Chercher le pool ou est pname.
	auto p = localClient.model().net().findPlace(std::string(pname));
	p->setTokenCount(pcount);
}

void handlePing(osc::ReceivedMessageArgumentStream args)
{
    remoteClients[0].send("/pong", localClient.id());
}

void handlePong(osc::ReceivedMessageArgumentStream args)
{
    osc::int32 idRemote;
    args >> idRemote;

    auto& rclient = remoteClients[idRemote];
    rclient.setLatency(getTime() - rclient.getPingTimestamp());

    std::cerr << "Latency : " << rclient.latency() << std::endl;
}
