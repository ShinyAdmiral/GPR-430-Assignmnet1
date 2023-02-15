#pragma once

class NetworkManager
{
public:
	NetworkManager(MessageLog& log) :
		messageLog(log)
	{
		SocketUtil::StaticInit();
	}

	~NetworkManager()
	{
		SocketUtil::CleanUp();
	}

	void Init();
	void CheckForNewConnections();
	void SendMessageToPeers(const std::string& message);
	void PostMessagesFromPeers();
	void AttemptToConnect(SocketAddressPtr targetAddress);

private:
	MessageLog& messageLog;
	TCPSocketPtr listenSocket;
	std::unordered_map<SocketAddress, TCPSocketPtr> openConnections;
	std::string defaultAddress = "127.0.0.1";
};