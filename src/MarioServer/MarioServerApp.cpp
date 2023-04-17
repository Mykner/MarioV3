#include "stdafx.h"

#include "MarioServer.h"
#include <thread>

void serverLoop(void * arg);

int main(int argc, char* argv[])
{
	// initialize the server
	MarioServer *server = new MarioServer;

	// create thread with arbitrary argument for the run function
	//std::thread *serverThread = new std::thread(serverLoop, (void*)server);
	//for ( ; ; ) ::Sleep(1000);
	serverLoop(server);
}

void serverLoop(void * arg)
{
	MarioServer *server = (MarioServer*)arg;

	while (true)
	{
		server->update();
	}
}