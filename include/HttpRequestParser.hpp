#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <string>
#include <sstream>
#include "server.hpp" // change "Server.hpp"

class HttpRequestParser {
public:
	HttpRequestParser();
	~HttpRequestParser();
	HttpRequestParser(const HttpRequestParser& other);
	HttpRequestParser& operator=(const HttpRequestParser& other);

	// Méthodes statiques pour parser différents types de requêtes
	static std::string getRequestType(const std::string& buffer);
	static void parseGetRequest(const std::string& buffer, Server& serv, int client_socket);
	static void parsePostRequest(const std::string& buffer, int client_socket, Server& serv);

private:
	// Méthodes privées pour le parsing interne
	static int _findContentLength(const std::string& buffer);
	static std::string _locateBoundary(const std::string& buffer);
	static bool _extractAndValidateFileName(const std::string& request, std::string& fileName);
	static bool _extractContentTypeAndData(const std::string& request, const std::string& boundary, std::string& contentType, std::string& fileContent);
};

#endif