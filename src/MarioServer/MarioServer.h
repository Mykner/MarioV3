#pragma once

#include "ServerNetwork.h"

class MarioServer
{
public:
	MarioServer(void);
	~MarioServer(void);

	void update();

private:
	// IDs for the clients connecting for table in ServerNetwork 
	static unsigned int client_id;

	// The ServerNetwork object 
	ServerNetwork* network;

	void receiveFromClients();

private:
	// data buffer
	char network_data[MAX_PACKET_SIZE];
};