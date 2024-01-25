#if 0
#include <iostream>
#include <enet/enet.h>
#include <string>

using namespace std;

int main(int argc, char ** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occured while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetHost* client;
	client = enet_host_create(NULL, 1, 1, 0, 0);

	if (client == NULL)
	{
		fprintf(stderr, "An error occured while trying to create an ENet client host!\n");
		return EXIT_FAILURE;
	}

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;

	enet_address_set_host(&address, "172.31.18.64");
	address.port = 2310;

	peer = enet_host_connect(client, &address, 1, 0);

	if (peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection\n");
		return EXIT_FAILURE;
	}

	if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to 127.0.0.1 : 7777 succeeded!");
	}
	else
	{
		enet_peer_reset(peer);
		puts("Connection to 127.0.0.1 : 7777 failed.");
		return EXIT_SUCCESS;
	}

	// Game loop start
	while (enet_host_service(client, &event, 1000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			// Packet client, string userInput & channelID
			Packet client;
			string userInput;
			int channelID = 0;
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->address.host,
				event.peer->address.port,
				event.channelID);

			// User message
			cout << "Enter a message to send to the server: ";
			getline(cin, userInput);
			ENetPacket* packet = enet_packet_create(userInput.c_str(), userInput.length(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, channelID, packet);
			break;
		}	
	}
	// Game loop end

	enet_peer_disconnect(peer, 0);

	while (enet_host_service(client, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			enet_packet_destroy(packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			break;
		}
	}
	return EXIT_SUCCESS;
}
#endif