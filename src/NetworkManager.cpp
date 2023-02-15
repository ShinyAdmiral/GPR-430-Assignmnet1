#include "RoboCatPCH.h"
#include "NetworkManager.h"
#include <iostream>

/// <summary>
/// Initialize listenSocket so it can receive incoming connections.
/// listenSocket must not block on any future calls.
/// </summary>
void NetworkManager::Init()
{
	listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	assert(listenSocket); //socket must be assigned

	//set up address
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(defaultAddress);
	assert(listenAddress); //listen address must be assigned

	//bind address to socket + error check
	if (listenSocket->Bind(*listenAddress) != NO_ERROR) {
		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}

	//set non blocking mode
	listenSocket->SetNonBlockingMode(true);

	messageLog.AddMessage("Hosting at: " + defaultAddress + ":" + std::to_string(listenSocket->GetPortNumber()));
}

/// <summary>
/// Called once per frame. Should check listenSocket for new connections.
/// If any come in, should add them to the list of openConnections, and
/// should send a message to each peer listing the addresses
/// of every other peer.
/// 
/// If there is a new connection, log some info about it with
/// messageLog.AddMessage().
/// </summary>
void NetworkManager::CheckForNewConnections()
{
	//listen for any new connections
	if (listenSocket->Listen() != NO_ERROR) {
		SocketUtil::ReportError("Listening on Listen Socket");
		messageLog.AddMessage("Could not listen on Socket");
		return;
	}

	//attempt to accept
	SocketAddress incominigAddress;
	TCPSocketPtr conSocket = listenSocket->Accept(incominigAddress);

	//for each new connection add a user
	while (conSocket != nullptr) {
		conSocket->SetNonBlockingMode(true);
		openConnections[incominigAddress] = conSocket;
		std::cout << "Accepted Connection from: " << incominigAddress.ToString() << std::endl;
		messageLog.AddMessage("Accepted Connection from: " + incominigAddress.ToString());
		conSocket = listenSocket->Accept(incominigAddress);
	}
}

/// <summary>
/// Sends the provided message to every connected peer. Called
/// whenever the user presses enter.
/// </summary>
/// <param name="message">Message to send</param>
void NetworkManager::SendMessageToPeers(const std::string& message)
{
	for (std::pair<SocketAddress, TCPSocketPtr> connection : openConnections){
		connection.second->Send(message.c_str(), message.length());
	}
}

void NetworkManager::PostMessagesFromPeers()
{
	for (std::pair<SocketAddress, TCPSocketPtr> connection : openConnections) {
		char buffer[4096];
		int32_t bytesReceived = connection.second->Receive(buffer, 4096);
		if (bytesReceived <= 0) continue;
		//while (bytesReceived <= 0)
		//{
		//	bytesReceived = connection.second->Receive(buffer, 4096);
		//}
		
		std::string receivedMsg(buffer, bytesReceived);
		messageLog.AddMessage(receivedMsg);
	}
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{
	assert(targetAddress);
	messageLog.AddMessage("Attempting Connection to: " + targetAddress->ToString());

	//set up connection socket
	SocketAddressPtr connectionAddress = SocketAddressFactory::CreateIPv4FromString(defaultAddress);
	assert(connectionAddress); //listen address must be assigned

	TCPSocketPtr connectionSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	//bind address to socket + error check
	if (connectionSocket->Bind(*connectionAddress) != NO_ERROR) {
		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}

	if (connectionSocket->Connect(*targetAddress) != NO_ERROR) {
		SocketUtil::ReportError("Connecting to Server");
		messageLog.AddMessage("Could not connect to server");
		return;
	}

	connectionSocket->SetNonBlockingMode(true);
	//add connection to the map whn successful
	openConnections[*connectionAddress] = connectionSocket;

	//connected to server
	std::string msg = "User Joined";
	SendMessageToPeers(msg);
}
