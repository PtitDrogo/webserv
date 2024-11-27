#include "HttpRequestParser.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>

HttpRequestParser::HttpRequestParser() {}
HttpRequestParser::~HttpRequestParser() {}

HttpRequestParser::HttpRequestParser(const HttpRequestParser& other) {
    (void)other;
}

HttpRequestParser& HttpRequestParser::operator=(const HttpRequestParser& other) {
    if (this != &other) {
        // Deep copy si nécessaire
    }
    return *this;
}

std::string HttpRequestParser::getRequestType(const std::string& buffer) {
    std::istringstream stream(buffer);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("GET") != std::string::npos)
            return "GET";
        if (line.find("POST") != std::string::npos)
            return "POST";
    }
    return "";
}

// Implémentez ici les méthodes statiques parseGetRequest, parsePostRequest, etc.
// Copiez-les depuis votre code précédent de parse_buffer_get et parse_buffer_post
// Ajustez le code pour utiliser les méthodes privées _findContentLength, etc.