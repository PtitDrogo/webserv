#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include <string>
#include <map>
#include <sys/socket.h>
#include <unistd.h>
#include "ClientUploadState.hpp"
#include "server.hpp" // change into "Server.hpp"
// #include "HttpRequestParser.hpp"

class ClientSocket {
public:
	ClientSocket();
	~ClientSocket();
	ClientSocket(const ClientSocket& other);
	ClientSocket& operator=(const ClientSocket& other);

	// methodes principales de gestion du socket client
	void handleNewClient(int clientSocket, Server& serv);
	void handleClientData(Server& serv);
	bool isValidSocket() const;
	void disconnect();

	// setters pour configurer le socket
	void setSocket(int socket);
	void setServer(Server* server);

	// getters
	int getSocket() const;
	ClientUploadState& getUploadState();

private:
	// attributs prives
	int 			  _socket;
	Server* 		  _server;
	ClientUploadState _uploadState;
	
	// methodes privees internes
	void _processNewRequest(const std::string& buffer);
	bool _receiveData();
	void _sendResponse(const std::string& response);
};

// classe pour gerer la collection de sockets clients
class ClientSocketManager {
public:
	void addClient(int socket, Server& serv);
	void removeClient(int socket);
	ClientSocket& getClient(int socket);
	bool hasClient(int socket) const;

private:
	std::map<int, ClientSocket> _clients;
};

#endif