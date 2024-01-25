#include <engine/Networking.h>
#include <engine/Application.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace engine;

ECS ecs;

struct PlayerData
{
	int playerID;
	Transform transform;
};


#define CLIENT
#ifdef CLIENT

int playerID;

bool OnClientConnect(int peerId)
{
	return true; // Accept the connection for demonstration purposes.
}
void OnClientDisconnect(int peerId)
{
}


int main(int argc, char** argv)
{
	NO_OPENAL = true;

	//Create the window and OpenGL context before creating EngineLib
	//Paraeters define window size(x,y) and name
	GLFWwindow* window = CreateGLWindow(1000, 1000, "Window");
	//Initialize the default engine library
	EngineLib engine;
	//Create the camera
	Camera cam = Camera(1000, 1000);
	cam.SetPosition(Vector3(0, 0, 20));
	SpriteRenderSystem::SetBackgroundColor(0, 120, 120);

	//Create entity
	Texture texture("assets/strawberry.png");
	Entity a = ecs.newEntity();
	Transform& aTransform = ecs.addComponent(a, Transform{ .position = Vector3(0, 0, -1), .rotation = Vector3(0, 0, 0), .scale = Vector3(50) });
	SpriteRenderer& aRenderer = ecs.addComponent(a, SpriteRenderer{ .texture = &texture });

	Entity b = ecs.newEntity();
	Transform& bTransform = ecs.addComponent(b, Transform{ .position = Vector3(100, 100, 0), .rotation = Vector3(0, 0, 45), .scale = Vector3(50) });
	SpriteRenderer& bRenderer = ecs.addComponent(b, SpriteRenderer{ .texture = &texture });

	//ENET create client
	std::string ipAddress = "127.0.0.1";
	uint16_t port = 2315;
	size_t channelLimit = 4;
	Connection client = CreateClient(ipAddress, port, channelLimit);

	auto OnClientReceive = [&bTransform](const Message& msg)
	{
		switch (msg.chanelID)
		{
		case 0:
			//Receive id info on 0
			playerID = Deserialize<int>(msg.data);
			break;
		case 1:
			//Receive other player's Transforms on 1
			PlayerData data = Deserialize<PlayerData>(msg.data);

			if (data.playerID != playerID)
				bTransform = data.transform;
			break;
		}

		return true;
	};

	float packetTimer = 0;

	//Game loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			TransformSystem::Translate(a, 0, -10, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			TransformSystem::Translate(a, 0, 10, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			TransformSystem::Translate(a, -10, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			TransformSystem::Translate(a, 10, 0, 0);
		}

		if (packetTimer <= 0)
		{
			//Send transform to server
			PlayerData data{ playerID, aTransform };
			SendData(client, data, 0);
			packetTimer = 0.05;
		}
		packetTimer -= engine.deltaTime;

		UpdateEnet(client, OnClientConnect, OnClientDisconnect, OnClientReceive, 10);

		engine.Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	enet_peer_disconnect(client.peers[0], 0);
	enet_host_destroy(client.host);

	enet_deinitialize();

	return EXIT_SUCCESS;
}

#else

Connection server;

void OnClientDisconnect(int peerId)
{
	std::cout << "Client disconnected with ID: " << peerId << std::endl;
}

bool OnClientReceive(const Message& msg)
{
	Transform t = deserialize<Transform>(msg.data);

	SendData(server, t, 1);

	return true;
}

bool OnClientConnect(int peerId)
{
	SendData(server, peerId, 0, peerId);

	std::cout << "Client connected with ID: " << peerId << std::endl;

	return true;
}

int main(int argc, char** argv)
{
	uint16_t port = 2315;
	size_t channelLimit = 4;
	size_t peerCount = 2;

	server = CreateHost(port, channelLimit, peerCount);

	//LOOP
	while (!server.closed)
	{
		UpdateEnet(server, OnClientConnect, OnClientDisconnect, OnClientReceive, 10);
	}
	//END LOOP

	close(server);
	std::cout << "Server closed." << std::endl;
	return EXIT_SUCCESS;
}
#endif