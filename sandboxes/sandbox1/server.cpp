//#include <iostream>
//#include <enet/enet.h>
//#include <stdio.h>
//#include "NetCode.h"
//
//bool OnClientConnect(int peerId) {
//	// Handle client connection. Return true if the connection is accepted, false otherwise.
//	return true; // Accept all connections for demonstration purposes.
//}
//
//void OnClientDisconnect(int peerId) {
//	// Handle client disconnection.
//	// You can implement custom disconnect logic here if needed.
//}
//
//bool OnClientReceive(const netcode::Message& msg) {
//	// Handle messages received from clients.
//	// You can implement custom message processing logic here.
//	printf("Received a message from client %d: ", msg.peerId);
//	for (const std::string& data : msg.rxData) {
//		printf("%s ", data.c_str());
//	}
//	printf("\n");
//
//	return true; // Return true to indicate that the message was processed successfully.
//}
//
//void SendPacket(netcode::Connection& conn, int peerId, int channelId, const std::vector<std::string>& msg) {
//	std::string fullMessage;
//	for (const auto& str : msg) {
//		fullMessage += str + ' ';
//	}
//
//	// Create an ENetPacket from the string
//	ENetPacket* packet = enet_packet_create(fullMessage.c_str(), fullMessage.size() + 1, ENET_PACKET_FLAG_RELIABLE);
//
//	// Send the packet to the specified peer
//	enet_host_broadcast(conn.host, channelId, packet);
//}
//
//int main(int argc, char** argv)
//{	
//	uint16_t port = 2315; 
//	size_t channelLimit = 1; 
//	size_t peerCount = 2; 
//
//	netcode::Connection server = netcode::createHost(port, channelLimit, peerCount);
//	ENetEvent event;
//	
//	//LOOP
//	while (!server.closed)
//	{
//		netcode::update(server, OnClientConnect, OnClientDisconnect, OnClientReceive, 50000);
//	}
//	//END LOOP
//
//	netcode::close(server);
//  std::cout << "Server closed." << std::endl;
//	return EXIT_SUCCESS;
//
//}