#pragma once
#include <iostream>
#include <enet/enet.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <assert.h>

namespace engine
{
	enum packetFlag
	{
		reliable = ENET_PACKET_FLAG_RELIABLE,
		unreliable = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT,
		unsequenced = ENET_PACKET_FLAG_UNSEQUENCED
	};

	///A packet to send over an enet connection
	struct Message
	{
		///The data in binary form
		char* data;
		///How much data there is for pointer math
		size_t dataLength = 0;
		///These are set automatically
		int peerID = -1;
		///These are set automatically
		int chanelID = 0;
	};

	///Struct to store connection data
	struct Connection
	{
		///Bool that turns the connection on and off
		bool closed = true;
		///Generates a host for the clients to connect to
		ENetHost* host = 0;
		///List of the clients
		std::vector<ENetPeer*> peers;
		///Reference to the Message struct
		Message msgBuffer;
		///Store the address
		ENetAddress address;

	};

	inline Connection CreateHost(uint16_t port, size_t channelLimit, size_t peerCount = 2)
	{
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "Error occurred while initializing\n");
			throw std::runtime_error("An error occurred while initializing ENet.");
		}
		atexit(enet_deinitialize);

		// Create client:
		Connection conn;
		conn.address.host = ENET_HOST_ANY;
		conn.address.port = port;
		conn.host = enet_host_create(&conn.address, peerCount, channelLimit, 0, 0);

		if (conn.host == NULL)
		{
			fprintf(stderr, "error while creating ENet server host.\n");
			throw std::runtime_error("An error occurred while creating ENet host.");
		}
		printf("Enet host created to port: %d\n", int(port));
		conn.closed = false;
		return conn;
	}

	inline Connection CreateClient(const std::string& ipAddress, uint16_t port, size_t channelLimit)
	{
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "Error occurred while initializing\n");
			throw std::runtime_error("An error occurred while initializing ENet.");
		}
		// Create client:
		Connection conn;

		enet_address_set_host(&conn.address, ipAddress.c_str());
		conn.address.port = port;
		conn.host = enet_host_create(NULL, 1, channelLimit, 0, 0);
		if (!conn.host)
		{
			throw std::runtime_error("Failed to initialize enet client.");
		}
		// Attempt to connect:
		auto peer = enet_host_connect(conn.host, &conn.address, channelLimit, 0);
		if (peer == nullptr)
		{
			throw std::runtime_error("No available peers at: " + ipAddress + ":" + std::to_string(port) + "\n");
		}

		conn.peers.push_back(peer);
		conn.closed = false;

		std::cout << "Successfully created client\n";
		return conn;
	}

	void CloseConnection(Connection& conn)
	{
		if (conn.host != 0)
		{
			printf("Destroy ENet connection. ");
			printf("Disconnect all peers...");
			for (auto peer : conn.peers)
			{
				printf(".");
				enet_peer_disconnect(peer, 0);
			}
			conn.peers.clear();
			// Run host once
			printf(" done!.\nWait to close connection...");
			ENetEvent event;
			enet_host_service(conn.host, &event, 50);
			printf(" closed!\n");
			enet_host_destroy(conn.host);
			printf("ENet connection destroyed.\n");
		}
		conn = Connection{};
	}

	//Serialize an aggregate data type into binary data
	//This is not null terminated, it's length will be the size of the type serialized
	template<typename T>
	char* Serialize(T data)
	{
		//Create the buffer to be the size of the data plus it's size denominator
		size_t size = sizeof(T);
		char* buff = new char[size];
		//Init the buffer to 0
		memset(buff, 0, size);
		//Copy the data to buffer
		memcpy(buff, &data, sizeof(T));

		return buff;
	}

	//Deserialize binary data into an object
	template<typename T>
	T Deserialize(char* data)
	{
		T result;
		memcpy(&result, data, sizeof(T));
		return result;
	}
	//Special case for string
	std::string DeserializeString(char* data, size_t size)
	{
		std::string result(data, size);
		return result;
	}

	//Update Enet, sends any queued packets, receives any pending packets, and calls the appropriate callbacs 
	inline void UpdateEnet(Connection& conn, auto connectFunc, auto disconnectFunc, auto receiveFunc, int timeOut)
	{
		//Get the update event from Enet
		ENetEvent event;
		if (enet_host_service(conn.host, &event, timeOut) < 0)
		{
			conn.closed = true;
			return;
		}

		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			char ipAddress[100];
			enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress) - 1);
			printf("Client connected to %s:%u.\n", ipAddress, event.peer->address.port);
			// Store client id:
			int peerId = int(conn.peers.size());
			conn.peers.push_back(event.peer);
			if (false == connectFunc(peerId))
			{
				enet_peer_reset(event.peer);
				conn.peers.pop_back();
			}
		}
		//Receiving data
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			//Parse event packet into Message struct
			Message msg;
			msg.chanelID = event.channelID;
			msg.peerID = -1;
			msg.dataLength = event.packet->dataLength;
			msg.data = new char[msg.dataLength];
			memcpy(msg.data, event.packet->data, msg.dataLength);

			//Packet no longer needed
			enet_packet_destroy(event.packet);

			//Send to appropriate peer
			for (int i = 0; i < conn.peers.size(); ++i)
			{
				if (conn.peers[i] == event.peer)
				{
					msg.peerID = i;
					//Make sure receive callback returns true
					if (!receiveFunc(msg))
					{
						printf("Peer %d misbehave!\n", conn.msgBuffer.peerID);
					}
				}
			}

			//Make sure to delete the heap data
			delete[] msg.data;
		}
		else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
		{
			for (size_t i = 0; i < conn.peers.size(); ++i)
			{
				if (conn.peers[i] == event.peer)
				{
					disconnectFunc(int(i));
					char ipAddress[100];
					enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress) - 1);
					printf("Client disconnected form: %s:%u.\n", ipAddress, event.peer->address.port);
					conn.peers.erase(conn.peers.begin() + i);
				}
			}
			if (conn.peers.size() == 0)
			{
				conn.closed = true;
			}
		}
	}

	//Create an enet packet from serialized data
	inline ENetPacket* CreatePacket(char* data, size_t size, packetFlag flags)
	{
		ENetPacket* packet = enet_packet_create(0, size, flags);
		memcpy(packet->data, data, size);
		return packet;
	}

	//This function takes a struct and sends it to the desired peer
	//ONLY AGGREGATE STRUCTS AND NO HEAP DATA
	template<typename T>
	int SendData(Connection& conn, T& data, int channelId, int peerId = -1, packetFlag packetFlag = packetFlag::reliable)
	{
		//Serialize the data to a bufer
		char* serializedData = Serialize(data);

		//Create an enet packet from the serialized data
		ENetPacket* packet = CreatePacket(serializedData, sizeof(T), packetFlag);

		//Serialized data not needed anymore
		delete serializedData;

		//Send to specific peer
		if (peerId >= 0)
			enet_peer_send(conn.peers[peerId], channelId, packet);
		//Send to all peers
		else
			enet_host_broadcast(conn.host, channelId, packet);

		return 0;
	}
	//Special case for string
	int SendData(Connection& conn, std::string& str, int channelId, int peerId = -1, packetFlag packetFlag = packetFlag::reliable)
	{
		//Create an enet packet from the string
		ENetPacket* packet = CreatePacket((char*)str.c_str(), str.size(), packetFlag);

		//Send to specific peer
		if (peerId >= 0)
			enet_peer_send(conn.peers[peerId], channelId, packet);
		//Send to all peers
		else
			enet_host_broadcast(conn.host, channelId, packet);

		return 0;
	}
}