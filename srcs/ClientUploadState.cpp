#include "ClientUploadState.hpp"

ClientUploadState::ClientUploadState() 
    : _contentLength(0), _bytesReceived(0), _headersParsed(false), 
      _receivedAll(false) {}

ClientUploadState::~ClientUploadState() {}

ClientUploadState::ClientUploadState(const ClientUploadState& other) 
    : _contentLength(other._contentLength),
      _bytesReceived(other._bytesReceived),
      _buffer(other._buffer),
      _headersParsed(other._headersParsed),
      _receivedAll(other._receivedAll),
      _boundary(other._boundary) {}

ClientUploadState& ClientUploadState::operator=(const ClientUploadState& other) {
    if (this != &other) {
        _contentLength = other._contentLength;
        _bytesReceived = other._bytesReceived;
        _buffer = other._buffer;
        _headersParsed = other._headersParsed;
        _receivedAll = other._receivedAll;
        _boundary = other._boundary;
    }
    return *this;
}

// Implémentez les getters et setters