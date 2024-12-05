#include "Client.hpp"

// Client::Client() 
// {}
// //: _socket(-1), _server() 

Client::~Client() 
{
	// std::cout << "Im destroying a client ! make sure this happens when you expect !" << std::endl;
}

Client::Client(const Client& other) : _socket(other._socket), _server(other._server), 
 _request(other._request), _body(other._body), _fileName(other._fileName),
 _contentType(other._contentType), _boundary(other._boundary), 
 _contentLength(other._contentLength), _totalRead(other._totalRead), _cgi_caller(other._cgi_caller)
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
		_request = other._request;
		_body = other._body;
		_fileName = other._fileName;
		_contentType = other._contentType;
		_boundary = other._boundary;
		_contentLength = other._contentLength;
		_totalRead = other._totalRead;
		_cgi_caller = other._cgi_caller;

	}
	return *this;
}

Client::Client(int clientSocket, Server& serv) : _socket(clientSocket), _server(serv), _request(),
 _body(), _fileName(), _contentType(), _boundary(), _contentLength(0), _totalRead(0), _cgi_caller(NULL)
{
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

// t_cgi_tmp_file&	Client::getCgiFilesFds() {return _cgi_fds;}

void    Client::appendToRequest(char *chunk, int recvValue) {
	_request.append(chunk, recvValue);
	_totalRead += recvValue;
	if (getContentLength() == 0) {
		setHeadEnd(getRequest().find("\r\n\r\n") + 4);
		std::cout << MAGENTA << "headEnd: " <<  getHeadEnd() << RESET << std::endl;	// debug print headEnd
		if (findContentLength() != 0) {
			setContentLength(findContentLength());

			// std::string test;												// debug print header
			// test = getRequest().substr(0, getHeadEnd() + 4);	// debug print header
			// std::cout << GREEN << "header: " << test << RESET << std::endl;	// debug print header

			setTotalRead(_totalRead - getHeadEnd() - 4);
			std::cout << MAGENTA << "content_length: " << getContentLength() << RESET << std::endl;	// debug print content_length
		}
	}
	std::cout << MAGENTA << "READ...: " << recvValue << "  " << getTotalRead() << RESET << std::endl;	// debug print totalRead
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

void	Client::setContentLength(size_t contentLength) { _contentLength = contentLength; }
void	Client::setTotalRead(size_t totalRead) { _totalRead = totalRead; }
void	Client::setHeadEnd(size_t headEnd) { _headEnd = headEnd; }
void	Client::setBody(std::string body) { _body = body; }

std::string	Client::getRequest() const { return (_request); }
std::string	Client::getBody() const{ return (_body); }
std::string	Client::getFileName() const { return (_fileName); }
std::string	Client::getContentType() const { return (_contentType); }
std::string	Client::getBoundary() const { return (_boundary); }
size_t		Client::getContentLength() const { return (_contentLength); }
size_t		Client::getTotalRead() const { return (_totalRead); }
size_t		Client::getHeadEnd() const { return (_headEnd); }


void Client::extractBody() {
	_body = _request.substr(0, getHeadEnd());
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

		size_t contentStart = _body.find("\r\n\r\n", contentTypeEnd);
		if (contentStart == std::string::npos) {
			std::cerr << "Error: Content start not found." << std::endl;
			return ; // try catch ???
		}
		contentStart += 4;

		// Pour Firefox, on soustrait 30 pour gérer correctement la fin du contenu
		size_t contentEnd = _body.find(_boundary + "--", contentStart); 
		if (contentEnd == std::string::npos) {
			std::cerr << "Error: Content end not found." << std::endl;
			return ; // try catch ???
		}

		std::string fileContent = _body.substr(contentStart, contentEnd - contentStart);

		std::ofstream outFile(_fileName.c_str(), std::ios::binary);
		if (!outFile) {
			std::cerr << "Error: Unable to create file: " << _fileName << std::endl;
			return ; // try catch ???
		}
		outFile.write(fileContent.data(), fileContent.size());
		outFile.close();
}
