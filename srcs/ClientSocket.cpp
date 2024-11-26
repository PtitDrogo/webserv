#include "ClientSocket.hpp"
#include <iostream>
#include <cstring>

// ClientSocket::ClientSocket() 
// {}
// //: _socket(-1), _server() 

ClientSocket::~ClientSocket() {
    if (_socket != -1) {
        close(_socket);
    }
}

ClientSocket::ClientSocket(const ClientSocket& other) 
    : _socket(other._socket), _server(other._server), 
      _uploadState(other._uploadState) {}

ClientSocket& ClientSocket::operator=(const ClientSocket& other) {
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

ClientSocket::ClientSocket(int clientSocket, Server& serv) :
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

void ClientSocket::_processNewRequest(const std::string& buffer) {
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

void ClientSocket::handleClientData(Server& serv) {
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

void ClientSocket::disconnect() {
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
}

bool ClientSocket::isValidSocket() const {
    return _socket != -1;
}

void ClientSocket::setSocket(int socket) {
    _socket = socket;
}

void ClientSocket::setServer(Server& server) {
    _server = server;
}

int ClientSocket::getSocket() const {
    return _socket;
}

ClientUploadState& ClientSocket::getUploadState() {
    return _uploadState;
}

// Implémentation de ClientSocketManager
void ClientSocketManager::addClient(int socket, Server& serv) {
    _clients[socket] = ClientSocket(socket, serv);
}

void ClientSocketManager::removeClient(int socket) {
    _clients.erase(socket);
}

ClientSocket& ClientSocketManager::getClient(int socket) {
    return _clients[socket];
}

bool ClientSocketManager::hasClient(int socket) const {
    return _clients.find(socket) != _clients.end();
}