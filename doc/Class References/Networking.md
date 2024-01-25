# Networking Reference

## Enet
This engine uses Enet for networking, that means that to host a server on the net you need to forward a UDP port.<br>
Enet had good documentation here: http://enet.bespin.org/Tutorial.html

---
## Client & Server

First you must create a **client.**
```cpp
//IP and port should be those of the server
std::string ipAddress = "127.0.0.1";
uint16_t port = 2315;
//How many communication channels can be used
size_t channelLimit = 4;
//Create the client, this returns a Connection struct with information about the ENet connection
Connection client = CreateClient(ipAddress, port, channelLimit);
```
Then you need a **server** to which all clients connect to. This doesn't have to be in a seperate executable.

```cpp
uint16_t port = 2315;
//How many communication channels can be used
size_t channelLimit = 4;
//The max client count
size_t peerCount = 2;
Connection server = CreateHost(port, channelLimit, peerCount);
```

Then you need to update ENet every frame which sends and fetches packets. It has three callbacks you need to provide: OnClientConnect, OnClientDisconnect, and OnClientReceive.<br>
There is also a timeout in milliseconds which determines how long to wait for any events. When only sending data this should be 0, but if you also expect to receive data it should be >0.

```cpp
//Define the callbacks, these could also be lambdas.
//The must be declared exactly like this with the same return types and arguments
bool OnClientConnect(int peerId)
{
	return true;
}
void OnClientDisconnect(int peerId)
{
}
bool OnClientReceive(const Message& msg)
{
	return true;
}

//Updates Enet calling all relevant callbacks, this usually should be called a maximum of once per frame
UpdateEnet(client, OnClientConnect, OnClientDisconnect, OnClientReceive, 10);
```

Finally, you have to close the connection after the game ends

```cpp
//Close the connection after game ends
close(connection);
enet_deinitialize();
```

---
## Sending Data

Enet expects data to be in binary format therefore you need to serialize everything before sending it. You can use the Serialize() function to serialize any aggregate struct which is contained entirely on the stack. To serialize anything else you need to write your own serialization function.<br>
The SendData() function calls Serialize() for you so you only need to give it the object though.

```cpp
//Send a Transform component to the server
Transform t{ .position = Vector3(100, 100, 0), .rotation = Vector3(0, 0, 45), .scale = Vector3(50) };
//Arguments are connection, data, channel number, peer id (-1 means all), and packet flags (reliable by default)
SendData(client, t, 0);
```

Strings are a special case since their data is on the heap. The SendData() function can accept them without issues.

## Receiving Data

Data is received when UpdateEnet() is called, it will then call the OnClientReceive callback with a Message struct which contains the data. You then need to deserialize it into the right type.

```cpp
//Callback
void OnClientReceive(const Message& msg)
{
	//Receive Transform on chanel 1
    if(msg.chanelID == 1)
	    Transform transform = Deserialize<Transform>(msg.data);
    //Receive string on chanel 2
    if(msg.chanelID == 2)
        //Strings are deserialized in a special way
        std::string str = DeserializeString(msg.data, msg.dataLength)

	return true;
};
```