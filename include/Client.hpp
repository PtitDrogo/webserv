#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Webserv.hpp"
// #include "HttpRequestParser.hpp"

class Client 
{
public:
	~Client();
	Client(int clientSocket, Server &serv); //only way a client can be constructed.
	Client(const Client& other);
	Client& operator=(const Client& other);

	
	// methodes principales de gestion du socket client
	// void handleNewClient(int clientSocket, Server& serv); //this function effectively doesnt exist or goes into the constructor
	void handleClientData(Server& serv);
	bool isValidSocket() const;
	void disconnect();

	// setters pour configurer le socket
	void setSocket(int socket);
	void setServer(Server& server);

	// getters
	Server& getServer() const;
	int getSocket() const;

	ClientUploadState& getUploadState();

private:
	Client(); //We cannot make a client without its server
	// attributs prives
	int 			  _socket;
	Server&			  _server;
	ClientUploadState _uploadState;
	// bool			  _isCGIPipe;
	// long long	  _timeStart; // init at -1;
	
	// methodes privees internes
	void _processNewRequest(const std::string& buffer);
	bool _receiveData();
	void _sendResponse(const std::string& response);
};

#endif