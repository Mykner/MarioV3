#include "stdafx.h"

#include "TestNetworkClient.h"

int main()
{
	ClientGame * client = new ClientGame();
	while (true)
	{
		// do game stuff
		client->update();
	}

    return 0;
}

