#ifndef CLIENT_UPLOAD_STATE_HPP
#define CLIENT_UPLOAD_STATE_HPP

#include <string>

class ClientUploadState {
public:
	ClientUploadState();
	~ClientUploadState();
	ClientUploadState(const ClientUploadState& other);
	ClientUploadState& operator=(const ClientUploadState& other);

	// Getters et setters
	int getContentLength() const;
	void setContentLength(int length);

	int getBytesReceived() const;
	void setBytesReceived(int bytes);

	std::string getBuffer() const;
	void appendToBuffer(const char* chunk, int length);

	bool areHeadersParsed() const;
	void setHeadersParsed(bool parsed);

	bool isReceivedAll() const;
	void setReceivedAll(bool received);

	std::string getBoundary() const;
	void setBoundary(const std::string& boundary);

private:
	int _contentLength;
	int _bytesReceived;
	std::string _buffer;
	bool _headersParsed;
	bool _receivedAll;
	std::string _boundary;
};

#endif