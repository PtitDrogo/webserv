#include "Client.hpp"

Client::~Client() 
{
	// std::cout << "Im destroying a client ! make sure this happens when you expect !" << std::endl;
}

Client::Client(const Client& other) : _socket(other._socket), _server(other._server), 
_uploadState(other._uploadState), _request(other._request), _contentLength(other._contentLength), 
_totalRead(other._totalRead), _cgi_caller(other._cgi_caller), _time_start(other._time_start), _cgi_callee(other._cgi_callee), _pid(other._pid)
{
	// std::cout << "client copy constructor called" << std::endl;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		// Fermer le socket existant si nécessaire
		if (_socket != -1) {
			close(_socket);
		}

		_socket = other._socket;
		_server = other._server;
		_uploadState = other._uploadState;
		_request = other._request;
		_contentLength = other._contentLength;
		_totalRead = other._totalRead;
		_time_start = other._time_start;
		_cgi_caller = other._cgi_caller;
		_cgi_callee = other._cgi_callee;
		_pid = other._pid;
	}
	return *this;
}

Client::Client(int clientSocket, Server& serv) : _socket(clientSocket), _server(serv), _uploadState(), 
_request(), _cgi_caller(NULL), _time_start(std::time(0)), _cgi_callee(NULL), _pid(-42)
{
	_contentLength = 0;
	_totalRead = 0;
	// std::cout << "Defaultish constructor called" << std::endl
	//There used to be stuff here, i think there should be nothing.
	// we shouldnt call recve when creating a child socket.
}

bool Client::isValidSocket() const {return _socket != -1;}


bool	Client::didClientTimeout() const
{
	if ((std::time(0) - _time_start) > CGI_TIMEOUT_SECONDS )
		return true;
	return false;
}



void    Client::appendToRequest(char *chunk, int recvValue) {
	_request.append(chunk, recvValue);
	_totalRead += recvValue;
	if (this->getContentLength() == 0 && this->findContentLength() != 0) {
		this->setContentLength(this->findContentLength());
		this->setHeadEnd(this->getRequest().find("\r\n\r\n"));
		// std::cout << MAGENTA << "headEnd: " <<  this->getHeadEnd() << RESET << std::endl;	// debug print headEnd

		// std::string test;												// debug print header
		// test = this->getRequest().substr(0, this->getHeadEnd() + 4);	// debug print header
		// std::cout << GREEN << "header: " << test << RESET << std::endl;	// debug print header

		this->setTotalRead(_totalRead - this->getHeadEnd() - 4);
		std::cout << MAGENTA << "content_length: " << this->getContentLength() << RESET << std::endl;	// debug print content_length
	}
	std::cout << MAGENTA << "READ...: " << recvValue << "  " << this->getTotalRead() << RESET << std::endl;	// debug print totalRead
}


size_t Client::findContentLength() {
	size_t      	content_length_pos = _request.find("Content-Length: ");
	if (content_length_pos == std::string::npos)
	{
		_contentLength = 0;
		return (_contentLength);
	}

	size_t length_start = content_length_pos + 16;
	size_t length_end = _request.find("\r\n", length_start);
	std::istringstream(_request.substr(length_start, length_end - length_start)) >> _contentLength;
	return (_contentLength);
}

void Client::setContentLength(size_t contentLength) {_contentLength = contentLength;}
void Client::setTotalRead(size_t totalRead) {_totalRead = totalRead;}
void Client::setHeadEnd(size_t headEnd) {_headEnd = headEnd;}
void Client::setBody(std::string body) {_body = body;}
void Client::setSocket(int socket) {_socket = socket;}
void Client::setServer(Server& server) {_server = server;}
void Client::setCgiPipeFD(int fd) {_cgi_fd = fd;}
void Client::setCgiCaller(Client *client_caller) {_cgi_caller = client_caller;}
void Client::setCgiCallee(Client *client_callee) {_cgi_callee = client_callee;}
void Client::setCgiPID(pid_t pid) {_pid = pid;}

std::string    Client::getRequest() const {return (_request);}
size_t	Client::getContentLength() const {return (_contentLength);}
size_t Client::getHeadEnd() const{return (_headEnd);}
size_t Client::getTotalRead() const {return (_totalRead);}
std::string Client::getBody() const{return (_body);}
int Client::getSocket() const {return _socket;}
Server& Client::getServer() const { return _server;}
Client* Client::getCgiCaller() const { return _cgi_caller;}
Client* Client::getCgiCallee() const { return _cgi_callee;}
long long	Client::getTimeStart() const { return _time_start;}
pid_t Client::getCgiPID() const { return _pid;}