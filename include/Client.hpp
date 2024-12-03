#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientUploadState.hpp"
#include "Webserv.hpp"

// #include "HttpRequestParser.hpp"


//This was an attempt at creating files.
// typedef struct s_cgi_tmp_file {
// 	int fd_in;
// 	int fd_out;
// 	int fd_err;
// 	s_cgi_tmp_file() : fd_in(-1), fd_out(-1), fd_err(-1) {}
// } t_cgi_tmp_file;
// t_cgi_tmp_file&	getCgiFilesFds() ;


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
	// void disconnect();

	// setters pour configurer le socket
	void setSocket(int socket);
	void setServer(Server& server);
	void setCgiPipeFD(int fd);
	void setCgiCaller(Client *client_caller);
	

	// getters
	Server& getServer() const;
	int getSocket() const;
	ClientUploadState& getUploadState();
	Client* getCgiCaller() const;


private:
	Client(); //We cannot make a client without its server
	// attributs prives
	int 			  _socket;
	Server&			  _server;
	ClientUploadState _uploadState;


	//CGI stuff
	int				  _cgi_fd; //where i can read the output of the cgi, later there should be more of these ??!
	Client			  *_cgi_caller;
	// long long	  _timeStart; // init at -1;
	
	// methodes privees internes
	// void _processNewRequest(const std::string& buffer);
	// bool _receiveData();
	// void _sendResponse(const std::string& response);
};

#endif