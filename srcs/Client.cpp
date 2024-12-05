#include "Client.hpp"

// Client::Client() 
// {}
// //: _socket(-1), _server() 

Client::~Client() 
{
	// std::cout << "Im destroying a client ! make sure this happens when you expect !" << std::endl;
}

Client::Client(const Client& other) : _socket(other._socket), _server(other._server), 
_uploadState(other._uploadState), _request(other._request), _contentLength(other._contentLength), 
_totalRead(other._totalRead), _cgi_caller(other._cgi_caller), _time_start(other._time_start)
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
	}
	return *this;
}

Client::Client(int clientSocket, Server& serv) : _socket(clientSocket), _server(serv), _uploadState(), _request(), _cgi_caller(NULL), _time_start(std::time(0)){
	_contentLength = 0;
	_totalRead = 0;
	// std::cout << "Defaultish constructor called" << std::endl
	//There used to be stuff here, i think there should be nothing.
	// we shouldnt call recve when creating a child socket.
}

//TFREYDIE NOTE : these methods call methods that dont exist, im assuming they exist somewhere else

// void Client::_processNewRequest(const std::string& buffer) {
//     std::string type_request = HttpRequestParser::getRequestType(buffer);

//     if (type_request == "GET") {
//         HttpRequestParser::parseGetRequest(buffer, _server, _socket);
//     } else if (type_request == "POST") {
//         // initialiser l'état du client pour POST
//         _uploadState.appendToBuffer(buffer.c_str(), buffer.length());
//         _uploadState.setBytesReceived(buffer.length());
		
//         handleClientData(_server);
//     } else {
//         std::cout << RED << "Error: Generation d'une page d'erreur pour les requetes non supportees" << RESET << std::endl;
//         // generate_html_page_error(*_server, _socket, "404");
//     }
// }

// void Client::handleClientData(Server& serv) {
//     char chunk[1024];
//     int bytesRead = recv(_socket, chunk, sizeof(chunk) - 1, 0);

//     if (bytesRead <= 0) {
//         disconnect();
//         return;
//     }

//     chunk[bytesRead] = '\0';
//     _uploadState.appendToBuffer(chunk, bytesRead);
//     _uploadState.setBytesReceived(_uploadState.getBytesReceived() + bytesRead);

//     // logique de parsing des en-tetes et du contenu
//     if (!_uploadState.areHeadersParsed()) {
//         size_t headerEnd = _uploadState.getBuffer().find("\r\n\r\n");
//         if (headerEnd == std::string::npos) {
//             return; // en-tetes pas encore complets
//         }

//         _uploadState.setHeadersParsed(true);
//     }

//     // verifier si toutes les donnees ont ete reçues
//     if (_uploadState.getBytesReceived() >= _uploadState.getContentLength()) {
//         HttpRequestParser::parsePostRequest(_uploadState.getBuffer(), _socket, serv);
//         disconnect();
//     }
// }

//presumably disconnect would need to also take itself away from the client MAP
// void Client::disconnect() {
//     if (_socket != -1) {
//         close(_socket);
//         _socket = -1;
//     }
// }

bool Client::isValidSocket() const {return _socket != -1;}

void Client::setSocket(int socket) {_socket = socket;}
void Client::setServer(Server& server) {_server = server;}
void Client::setCgiPipeFD(int fd) {_cgi_fd = fd;}
void Client::setCgiCaller(Client *client_caller) {_cgi_caller = client_caller;}


int Client::getSocket() const {return _socket;}
Server& Client::getServer() const { return _server;}
Client* Client::getCgiCaller() const { return _cgi_caller;}
long long	Client::getTimeStart() const { return _time_start;}

// t_cgi_tmp_file&	Client::getCgiFilesFds() {return _cgi_fds;}

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

std::string    Client::getRequest() const {
	return (_request);
}

void	Client::setContentLength(size_t contentLength) {
	_contentLength = contentLength;
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

size_t	Client::getContentLength() const {
	return (_contentLength);
}

size_t Client::getTotalRead() const {
	return (_totalRead);
}

void Client::setTotalRead(size_t totalRead) {
	_totalRead = totalRead;
}

void Client::setHeadEnd(size_t headEnd) {
	_headEnd = headEnd;
}

size_t Client::getHeadEnd() const{
	return (_headEnd);
}

void Client::setBody(std::string body) {
	_body = body;
}

std::string Client::getBody() const{
	return (_body);
}


// bool Client::extractFileName() {
// 	const std::string key = "filename=\"";

// 	size_t fileNamePos = body.find(key);

// 	if (fileNamePos == std::string::npos) {
// 		std::cerr << "Error: Filename not found." << std::endl;
// 		return false;
// 	}

// 	size_t endPos = body.find("\"\r\n", fileNamePos);
// 	if (endPos == std::string::npos) {
// 		std::cerr << "Error: Invalid filename format." << std::endl;
// 		return false;
// 	}

// 	std::string fileName = body.substr(fileNamePos + key.length(), endPos - (fileNamePos + key.length()));

// 	// // Validation du nom de fichier pour éviter les chemins traversants
// 	// if (fileName.find("/") != std::string::npos || fileName.find("..") != std::string::npos) {
// 	// 	std::cerr << "Error: Invalid filename." << std::endl;
// 	// 	return false;
// 	// }

// 	fileName = "./config/base_donnees/" + fileName;
// }