#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientUploadState.hpp"
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
	void	handleClientData(Server& serv);
	bool	isValidSocket() const;
	void	appendToRequest(char *chunk, int recvValue);
	size_t	findContentLength();
	bool	extractFileName();
	// void disconnect();

	// setters
	void	setSocket(int socket);
	void	setServer(Server& server);
	void	setContentLength(size_t contentLength);
	void	setTotalRead(size_t totalRead);
	void	setHeadEnd(size_t heanEnd);
	void	setBody(std::string body);

	// getters
	Server&		getServer() const;
	int			getSocket() const;
	std::string	getRequest() const;
	size_t		getContentLength() const;
	size_t		getTotalRead() const;
	size_t		getHeadEnd() const;
	std::string getBody() const;

	ClientUploadState& getUploadState();

private:
	Client(); //We cannot make a client without its server
	// attributs prives
	int 			 	_socket;
	Server&			 	_server;
	ClientUploadState	_uploadState;

	std::string			_request;
	std::string			_body;
	size_t				_contentLength;
	size_t				_totalRead;
	size_t				_headEnd;
	// bool			  _isCGIPipe;
	// long long	  _timeStart; // init at -1;
	
	// methodes privees internes
	// void _processNewRequest(const std::string& buffer);
	// bool _receiveData();
	// void _sendResponse(const std::string& response);
};

#endif