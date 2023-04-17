#include "stdafx.h" 
#include "MarioServer.h"

unsigned int MarioServer::client_id;

MarioServer::MarioServer(void)
{
	// id's to assign clients for our table
	client_id = 0;

	// set up the server network to listen 
	network = new ServerNetwork();
}

void MarioServer::update()
{
	// get new clients
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);

		client_id++;
	}

	receiveFromClients();
}

void MarioServer::receiveFromClients()
{
	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		// get data for that client
		int data_length = network->receiveData(iter->first, network_data);
		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);

			switch (packet.packet_type) 
			{
			case INIT_CONNECTION:
				printf("server received init packet from client\n");
				break;
			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}