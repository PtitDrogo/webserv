#include "Client.hpp"

Client::~Client() 
{
	// std::cout << "Im destroying a client ! make sure this happens when you expect !" << std::endl;
}

Client::Client(const Client& other) : _socket(other._socket), _server(other._server), 
 _request(other._request), _body(other._body), _fileName(other._fileName), _contentType(other._contentType), _boundary(other._boundary), 
 _contentLength(other._contentLength), _totalRead(other._totalRead), _bodyEnd(other._bodyEnd), _cgi_caller(other._cgi_caller),
 _time_start(other._time_start), _cgi_callee(other._cgi_callee), _pid(other._pid)
{
	_current_location = other._current_location;
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
		_request = other._request;
		_body = other._body;
		_fileName = other._fileName;
		_contentType = other._contentType;
		_boundary = other._boundary;
		_contentLength = other._contentLength;
		_totalRead = other._totalRead;
		_bodyEnd = other._bodyEnd;
		_time_start = other._time_start;
		_cgi_caller = other._cgi_caller;
		_cgi_callee = other._cgi_callee;
		_pid = other._pid;
		_current_location = other._current_location;
	}
	return *this;
}

Client::Client(int clientSocket, Server& serv) : _socket(clientSocket), _server(serv), _contentLength(0), _totalRead(0), 
_bodyEnd(0), _current_location(NULL), _cgi_caller(NULL), _time_start(std::time(0)), _cgi_callee(NULL), _pid(-42)
{
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
	if (getContentLength() == 0) {
		setHeadEnd(getRequest().find("\r\n\r\n") + 4);
		// std::cout << MAGENTA << "headEnd: " <<  getHeadEnd() << RESET << std::endl;	// debug print headEnd
		if (findContentLength() != 0) {
			setContentLength(findContentLength());

			// std::string test;												// debug print header
			// test = getRequest().substr(0, getHeadEnd() + 4);	// debug print header
			// std::cout << GREEN << "header: " << test << RESET << std::endl;	// debug print header

			setTotalRead(_totalRead - getHeadEnd() /* - 4 */);
			// std::cout << MAGENTA << "content_length: " << getContentLength() << RESET << std::endl;	// debug print content_length
		}
	}
	// std::cout << MAGENTA << "READ...: " << recvValue << "  " << getTotalRead() << RESET << std::endl;	// debug print totalRead
}


size_t Client::findContentLength() {
	size_t	content_length_pos = _request.find("Content-Length: ");
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




void Client::extractBody() {
	_body = _request.substr(getHeadEnd(), getBodyEnd() - getHeadEnd());
}

void Client::extractFileName() {
	const std::string key = "filename=\"";

	extractBody();
	size_t fileNamePos = getBody().find(key);

	if (fileNamePos == std::string::npos) {
		std::cerr << "Error: Filename not found." << std::endl;
		return ; // try/catch ??
	}

	size_t endPos = getBody().find("\"\r\n", fileNamePos);
	if (endPos == std::string::npos) {
		std::cerr << "Error: Invalid filename format." << std::endl;
		return ; // try/catch ??
	}

	std::string fileName = getBody().substr(fileNamePos + key.length(), endPos - (fileNamePos + key.length()));

	// // Validation du nom de fichier pour éviter les chemins traversants
	// if (fileName.find("/") != std::string::npos || fileName.find("..") != std::string::npos) {
	// 	std::cerr << "Error: Invalid filename." << std::endl;
	// 	return false;
	// }

	fileName = "./config/base_donnees/" + fileName;
	_fileName = fileName;
}

void Client::extractContentType() {
	size_t contentTypePos = _body.find("Content-Type:", _body.find("--" + _boundary));
	if (contentTypePos == std::string::npos) {
		std::cerr << "Error: Content-Type not found." << std::endl;
		return ; // try catch ???
	}

	size_t contentTypeEnd = _body.find("\r\n", contentTypePos);
	if (contentTypeEnd == std::string::npos) {
		std::cerr << "Error: Malformed Content-Type header." << std::endl;
		return ; // try catch ???
	}
	std::string contentType = _body.substr(contentTypePos + std::string("Content-Type: ").length(), contentTypeEnd - (contentTypePos + std::string("Content-Type: ").length()));

	std::cout << MAGENTA << "contentType: \"" << contentType << "\"" << RESET << std::endl;


	std::string filecontent = _body.substr(contentTypeEnd + 4, _body.size() - (contentTypeEnd + 4));


	std::ofstream outFile(_fileName.c_str(), std::ios::binary);
	if (!outFile) {
		std::cerr << "Error: Unable to create file: " << _fileName << std::endl;
		return ; // try catch ???
	}
	outFile.write(filecontent.data(), filecontent.size());
	outFile.close();
}

void	Client::reset() {
	_request.clear();
	_body.clear();
	_fileName.clear();
	_contentType.clear();
	_boundary.clear();
	_contentLength = 0;
	_totalRead = 0;
	_headEnd = 0;
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
void Client::setLocation(location *location) {_current_location = location;}
void	Client::setBoundary(std::string boundary) { _boundary = boundary; }
void	Client::setbodyEnd(size_t bodyEnd) { _bodyEnd = bodyEnd; }
void	Client::setToken(std::string token) {_token = token;}

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
location *Client::getLocation() const {return _current_location;}
std::string	Client::getFileName() const { return (_fileName); }
std::string	Client::getContentType() const { return (_contentType); }
std::string	Client::getBoundary() const { return (_boundary); }
size_t		Client::getBodyEnd() const { return (_bodyEnd); }
std::string	Client::getToken() const {return (_token);}