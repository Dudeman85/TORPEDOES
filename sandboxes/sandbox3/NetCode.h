#pragma once
#include <iostream>
#include <enet/enet.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>


namespace netcode
{
	//A packet to send over an enet connection
	struct Packet
	{
		int peerID;
		int chanelID;
		//The type of data being sent
		std::string type;
		//The data in binary form
		std::vector<std::string> data;
	};

	struct Connection
	{
		bool closed = true;
		ENetHost* host = 0;
		std::vector<ENetPeer*> peers;
		Packet msgBuffer;
		ENetAddress address;

	};

	inline Connection createHost(uint16_t port, size_t channelLimit, size_t peerCount = 2)
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



	inline Connection createClient(const std::string& ipAddress, uint16_t port, size_t channelLimit)
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

	void close(netcode::Connection& conn)
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

	inline void update(netcode::Connection& conn, auto connectFunc, auto disconnectFunc, auto receiveFunc, int timeOut)
	{
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
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			// Parse message:
			conn.msgBuffer.chanelID = event.channelID;
			conn.msgBuffer.peerID = -1;
			conn.msgBuffer.data.clear();
			for (size_t i = 0; i < event.packet->dataLength; )
			{
				std::string str((const char*)&event.packet->data[i]);
				conn.msgBuffer.data.push_back(str);
				i += str.length() + 1;
			}
			enet_packet_destroy(event.packet);
			// Send to appropriate peer:
			for (size_t i = 0; i < conn.peers.size(); ++i)
			{
				if (conn.peers[i] == event.peer)
				{
					conn.msgBuffer.peerID = int(i);
					if (!receiveFunc(conn.msgBuffer))
					{
						printf("Peer %d misbehave!\n", conn.msgBuffer.peerID);
					}
				}
			}
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

	//Serialize an aggregate data type
	template<typename T>
	char* serialize(T data)
	{
		char* buff = new char[sizeof(T)];
		//memcpy(buff, &data, );
	}

	inline ENetPacket* createPacket(const std::vector<std::string>& msg, ENetPacketFlag flags)
	{
		size_t msgSize = 0;
		for (const auto& s : msg)
		{
			msgSize += s.length() + 1;
		}
		ENetPacket* packet = enet_packet_create(0, msgSize, flags);
		size_t i = 0;
		for (const auto& s : msg)
		{
			memcpy(packet->data + i, s.c_str(), s.length() + 1);
			i += s.length() + 1;
		}
		return packet;
	}

	inline void sendReliableTo(netcode::Connection& conn, int peerId, int channelId, const std::vector<std::string>& msg)
	{
		enet_peer_send(conn.peers[peerId], channelId, createPacket(msg, ENET_PACKET_FLAG_RELIABLE));
	}

	inline void sendReliable(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg)
	{
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_RELIABLE));
	}

	inline void sendUnreliable(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg)
	{
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));
	}

	inline void sendUnsequenced(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg)
	{
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_UNSEQUENCED));
	}


}