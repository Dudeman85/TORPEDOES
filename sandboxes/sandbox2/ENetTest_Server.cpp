#if 0
#include <iostream>
#include <cstring>
#include <string>
#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
#endif
#include <enet/enet.h>


/*
* Reading ENet theory and looking at teachers netgame example.
*/


int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occured while initializing ENet!\n");
		return EXIT_FAILURE;
	}

	atexit(enet_deinitialize);

	ENetAddress address;
	ENetEvent event;
	ENetHost* server;

	address.host = ENET_HOST_ANY;
	address.port = 7777;

	
	server = enet_host_create(&address, 32, 1, 0, 0);

	if (server == NULL)
	{
		fprintf(stderr, "An error occured while trying to create an ENet server host!\n");
		return EXIT_FAILURE;
	}

	//const char* responseMessage = "Hello from the server!";
	//size_t responseMessageLength = std::strlen(responseMessage);
	//ENetPacket* responsePacket = enet_packet_create(responseMessage, responseMessageLength, ENET_PACKET_FLAG_RELIABLE);

	// Game loop start
	while (true)
	{
		while (enet_host_service(server, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x : %u.\n",
					event.peer->address.host,
					event.peer->address.port);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %u containing %s was received from %s on channel %u.\n",
					event.packet->dataLength,
					event.packet->data,
					event.peer->address.host,
					event.peer->address.port,
					event.channelID);

				// Server Packet
				//enet_peer_send(event.peer, event.channelID, responsePacket);

				//enet_packet_destroy(responsePacket);

				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x : %u disconnected.\n",
					event.peer->address.host,
					event.peer->address.port);
				break;
			}
		}
	}
	// Game loop end

	enet_host_destroy(server);

	return EXIT_SUCCESS;
}
#endif