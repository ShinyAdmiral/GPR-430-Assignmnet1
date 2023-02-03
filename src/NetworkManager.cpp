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

	//set non blocking mode
	listenSocket->SetNonBlockingMode(true);

	//set up address
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("216.93.149.19:8080");
	assert(listenAddress); //listen address must be assigned

	//bind address to socket + error check
	if (listenSocket->Bind(*listenAddress) != NO_ERROR) {

		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}
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
	if (listenSocket->Listen() != NO_ERROR) {
		SocketUtil::ReportError("Listening on Listen Socket");
		return;
	}

	SocketAddress incominigAddress;
	TCPSocketPtr conSocket = listenSocket->Accept(incominigAddress);

	while (conSocket != nullptr) {
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

}

void NetworkManager::PostMessagesFromPeers()
{
	
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{

}
