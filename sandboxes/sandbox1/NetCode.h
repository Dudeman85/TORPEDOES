#pragma once
#include <iostream>
#include <enet/enet.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>


namespace netcode {

	struct Message {
		int peerId;						// Sender peer id.
		int msgId;						// Channel/MessageID
		std::vector<std::string> rxData; // Receive payload data
		std::vector<std::string> txData; // Send payload data
	};

	struct Connection {
		bool closed = true;
		ENetHost* host = 0;
		std::vector<ENetPeer*> peers;
		Message msgBuffer;
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
		if (enet_initialize() != 0) {
			fprintf(stderr, "Error occurred while initializing\n");
			throw std::runtime_error("An error occurred while initializing ENet.");
		}
		// Create client:
		Connection conn;
		
		enet_address_set_host(&conn.address, ipAddress.c_str());
		conn.address.port = port;
		conn.host = enet_host_create(NULL, 1, channelLimit, 0, 0);
		if (!conn.host) {
			throw std::runtime_error("Failed to initialize enet client.");
		}
		// Attempt to connect:
		auto peer = enet_host_connect(conn.host, &conn.address, channelLimit, 0);
		if (peer == nullptr) {
			throw std::runtime_error("No available peers at: " + ipAddress + ":" + std::to_string(port) + "\n");
		}

		conn.peers.push_back(peer);
		conn.closed = false;

		std::cout << "Successfully created client\n";
		return conn;
	}

	void close(netcode::Connection& conn) {
		if (conn.host != 0) {
			printf("Destroy ENet connection. ");
			printf("Disconnect all peers...");
			for (auto peer : conn.peers) {
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

	inline void update(netcode::Connection& conn, auto connectFunc, auto disconnectFunc, auto receiveFunc, int timeOut) {
		ENetEvent event;
		if (enet_host_service(conn.host, &event, timeOut) < 0) {
			conn.closed = true;
			return;
		}

		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			char ipAddress[100];
			enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress) - 1);
			printf("Client connected to %s:%u.\n", ipAddress, event.peer->address.port);
			// Store client id:
			int peerId = int(conn.peers.size());
			if (false == connectFunc(peerId)) {
				enet_peer_reset(event.peer);
			}
			else {
				conn.peers.push_back(event.peer);
			}
		}
		else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			// Parse message:
			conn.msgBuffer.msgId = event.channelID;
			conn.msgBuffer.peerId = -1;
			conn.msgBuffer.rxData.clear();
			for (size_t i = 0; i < event.packet->dataLength; ) {
				std::string str((const char*)&event.packet->data[i]);
				conn.msgBuffer.rxData.push_back(str);
				i += str.length() + 1;
			}
			enet_packet_destroy(event.packet);
			// Send to appropriate peer:
			for (size_t i = 0; i < conn.peers.size(); ++i) {
				if (conn.peers[i] == event.peer) {
					conn.msgBuffer.peerId = int(i);
					if (!receiveFunc(conn.msgBuffer)) {
						printf("Peer %d misbehave!\n", conn.msgBuffer.peerId);
					}
				}
			}
		}
		else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
			for (size_t i = 0; i < conn.peers.size(); ++i) {
				if (conn.peers[i] == event.peer) {
					disconnectFunc(int(i));
					char ipAddress[100];
					enet_address_get_host_ip(&event.peer->address, ipAddress, sizeof(ipAddress) - 1);
					printf("Client disconnected form: %s:%u.\n", ipAddress, event.peer->address.port);
					conn.peers.erase(conn.peers.begin() + i);
				}
			}
			if (conn.peers.size() == 0) {
				conn.closed = true;
			}
		}
	}




	inline ENetPacket* createPacket(const std::vector<std::string>& msg, ENetPacketFlag flags) {
		size_t msgSize = 0;
		for (const auto& s : msg) {
			msgSize += s.length() + 1;
		}
		ENetPacket* packet = enet_packet_create(0, msgSize, flags);
		size_t i = 0;
		for (const auto& s : msg) {
			memcpy(packet->data + i, s.c_str(), s.length() + 1);
			i += s.length() + 1;
		}
		return packet;
	}

	inline void sendReliableTo(netcode::Connection& conn, int peerId, int channelId, const std::vector<std::string>& msg) {
		enet_peer_send(conn.peers[peerId], channelId, createPacket(msg, ENET_PACKET_FLAG_RELIABLE));
	}

	inline void sendReliable(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg) {
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_RELIABLE));
	}

	inline void sendUnreliable(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg) {
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));
	}

	inline void sendUnsequenced(netcode::Connection& conn, int channelId, const std::vector<std::string>& msg) {
		enet_host_broadcast(conn.host, channelId, createPacket(msg, ENET_PACKET_FLAG_UNSEQUENCED));
	}


}

/*
struct Foo {
	uint16_t val1;
	uint16_t val2;
};

Foo foo;
std::vector<uint8_t> buffer;
buffer.resize(sizeof(foo));
memcpy(&buffer[0], &foo, sizeof(foo));

uint16_t val;
buffer.resize(2);

#define PUT16(buf, v) buf[0] = (v & 0x00ff); buf[1] = (val & 0xff00) >> 8;


PUT16(buffer, val);

template<typename T>
void serialize(std::vector<uint8_t> buffer, T msg);

template<>
void serialize(std::vector<uint8_t> buffer, Foo msg) {
	PUT16(&buffer[0], msg.val1);
	PUT16(&buffer[2], msg.val2);
}


template<typename T>
void sendMessage(T msg) {
	auto buf = allocPacket(4);
	serialize(buf, msg);
	sendPacket(buf);
}


netConnectrion.sendMessage<Foo>({0, 1});
*/

/// paketti struktiin; pacet id, packet type, data-char array

// struktista bin‰‰riksi, bin‰‰rin purku takaisin stuktiksi, memcopy

//funktio struktin muuttamiseksi