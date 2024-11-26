#include "Client.hpp"

// Client::Client() 
// {}
// //: _socket(-1), _server() 

Client::~Client() {
    if (_socket != -1) {
        close(_socket);
    }
}

Client::Client(const Client& other) 
    : _socket(other._socket), _server(other._server), 
      _uploadState(other._uploadState) {}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        // Fermer le socket existant si nécessaire
        if (_socket != -1) {
            close(_socket);
        }

        _socket = other._socket;
        _server = other._server;
        _uploadState = other._uploadState;
    }
    return *this;
}

Client::Client(int clientSocket, Server& serv) :
_socket(-1), _server(serv), _uploadState()
{
    _socket = clientSocket;
    _server = serv;

    char buffer[1024] = {0};
    int recv_value = recv(_socket, buffer, sizeof(buffer) - 1, 0);

    if (recv_value <= 0) {
        disconnect();
        return;
    }

    _processNewRequest(std::string(buffer));
}

void Client::_processNewRequest(const std::string& buffer) {
    std::string type_request = HttpRequestParser::getRequestType(buffer);

    if (type_request == "GET") {
        HttpRequestParser::parseGetRequest(buffer, _server, _socket);
    } else if (type_request == "POST") {
        // initialiser l'état du client pour POST
        _uploadState.appendToBuffer(buffer.c_str(), buffer.length());
        _uploadState.setBytesReceived(buffer.length());
        
        handleClientData(_server);
    } else {
        std::cout << RED << "Error: Generation d'une page d'erreur pour les requetes non supportees" << RESET << std::endl;
        // generate_html_page_error(*_server, _socket, "404");
    }
}

void Client::handleClientData(Server& serv) {
    char chunk[1024];
    int bytesRead = recv(_socket, chunk, sizeof(chunk) - 1, 0);

    if (bytesRead <= 0) {
        disconnect();
        return;
    }

    chunk[bytesRead] = '\0';
    _uploadState.appendToBuffer(chunk, bytesRead);
    _uploadState.setBytesReceived(_uploadState.getBytesReceived() + bytesRead);

    // logique de parsing des en-tetes et du contenu
    if (!_uploadState.areHeadersParsed()) {
        size_t headerEnd = _uploadState.getBuffer().find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            return; // en-tetes pas encore complets
        }

        _uploadState.setHeadersParsed(true);
    }

    // verifier si toutes les donnees ont ete reçues
    if (_uploadState.getBytesReceived() >= _uploadState.getContentLength()) {
        HttpRequestParser::parsePostRequest(_uploadState.getBuffer(), _socket, serv);
        disconnect();
    }
}

void Client::disconnect() {
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
}

bool Client::isValidSocket() const {
    return _socket != -1;
}

void Client::setSocket(int socket) {
    _socket = socket;
}

void Client::setServer(Server& server) {
    _server = server;
}

int Client::getSocket() const {
    return _socket;
}
Server& Client::getServer() const { return _server;}

ClientUploadState& Client::getUploadState() {
    return _uploadState;
}