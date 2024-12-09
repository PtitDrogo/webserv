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
	void	handleClientData(Server& serv);
	bool	isValidSocket() const;
	void	appendToRequest(char *chunk, int recvValue);
	size_t	findContentLength();
	bool	extractFileName();
	// void disconnect();
	bool	didClientTimeout() const;


	// setters
	void	setSocket(int socket);
	void	setServer(Server& server);
	void	setContentLength(size_t contentLength);
	void	setTotalRead(size_t totalRead);
	void	setHeadEnd(size_t heanEnd);
	void	setBody(std::string body);
	void 	setCgiPipeFD(int fd);
	void 	setCgiCaller(Client *client_caller);
	void 	setCgiCallee(Client *client_caller);
	void 	setCgiPID(pid_t pid);
	void	setLocation(location *location);

	// getters
	Server&		getServer() const;
	int			getSocket() const;
	std::string	getRequest() const;
	size_t		getContentLength() const;
	size_t		getTotalRead() const;
	size_t		getHeadEnd() const;
	std::string getBody() const;
	Client* 	getCgiCaller() const;
	Client* 	getCgiCallee() const;
	long long	getTimeStart() const;
	pid_t		getCgiPID() const;
	location	*getLocation() const;


private:
	Client(); //We cannot make a client without its server

	int 			 	_socket;
	Server&			 	_server;
	ClientUploadState	_uploadState;

	std::string			_request;
	std::string			_body;
	size_t				_contentLength;
	size_t				_totalRead;
	size_t				_headEnd;

	
	//this really should be in the request class but we are not using it, so now its here;
	//SET THIS BACK TO NULL AFTER EACH RESPONSE OTHERWISE IT WILL ALWAYS THINK ITS IN LOCATION;
	location			*_current_location;
	
	//CGI stuff
	int				  _cgi_fd; //where i can read the output of the cgi, later there should be more of these ??!
	Client			  *_cgi_caller;
	long long	  	  _time_start;
	Client			  *_cgi_callee;
	pid_t			  _pid;
	// methodes privees internes
	// void _processNewRequest(const std::string& buffer);
	// bool _receiveData();
	// void _sendResponse(const std::string& response);
};

#endif