//#include <iostream>
//#include <enet/enet.h>
//#include <stdio.h>
//#include <string.h>
//#include "NetCode.h"
//#include <chrono>
//#include <thread>
//
//bool OnClientConnect(int peerId) {
//	// Handle client connection.
//	std::cout << "connected";
//	return true; // Accept the connection for demonstration purposes.
//}
//
//void OnClientDisconnect(int peerId) {
//	// Handle client disconnection.
//	// You can implement custom disconnect logic here if needed.
//	std::cout << "disconnected";
//}
//
//bool OnClientReceive(const netcode::Message& msg) {
//	// Handle messages received from the server.
//	// You can implement custom message processing logic here.
//	printf("(Client) Received a message from the server: ");
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
//	if (packet == nullptr) {
//		printf("Failed to create packet!\n");
//		return;
//	}
//	// Send the packet to the specified peer
//	//enet_peer_send(conn.peers[peerId], channelId, packet);
//	std::cout << enet_peer_send(conn.peers[peerId], channelId, packet);
//}
//
//void ZeroPacket(netcode::Connection& conn, int peerId, int channelId) {
//
//	ENetPacket* zero = enet_packet_create(nullptr, 0, ENET_PACKET_FLAG_RELIABLE);
//	if (zero == nullptr) {
//		printf("Failed to create empty packet!\n");
//		return;
//	}
//}
//
//int main(int argc, char** argv)
//{
//	std::string ipAddress = "172.31.18.64";
//	uint16_t port = 2315;
//	size_t channelLimit = 4;
//	netcode::Connection client = netcode::createClient(ipAddress, port, channelLimit);
//	ENetEvent event;
//
//	bool playerTurn;
//
//
//
//	std::vector<std::string> message = { "Hello, this is a test message." };
//	int channelId = 0; // The channel on which to send the message
//	
//	
//
//	//LOOP
//	while (!client.closed)
//	{
//		netcode::update(client, OnClientConnect, OnClientDisconnect, OnClientReceive, 15000);
//		//std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//		SendPacket(client, 0, channelId, message);
//		ZeroPacket(client, 0, channelId);
//		
//	}
//	//END LOOP
//	
//	netcode::close(client);
//	std::cout << "Client closed." << std::endl;
//	return EXIT_SUCCESS;
//}