#include "Webserv.hpp"

std::string parse_request(std::string type, std::string buffer, HttpRequest &req)
{
    std::istringstream stream(buffer);
    std::string line;
    std::string method;
    std::string path;
    std::string version;
    std::string body;
    while (std::getline(stream, line))
    {
        size_t pos1 = line.find(type.c_str());
        size_t pos2 = line.find("HTTP");

        if (pos1 != std::string::npos && pos2 != std::string::npos)
        {
            method = line.substr(pos1, type.size() + 1);
            req.setMethod(method);
            path = line.substr(pos1 + type.size() + 1, pos2 - pos1 - (type.size() + 2));
            req.setPath(path);
            version = line.substr(pos2);
            req.setVersion(version);
        }
        body += line + "\n";
    }
    req.setBody(body);
    return (type);
}

std::string get_type_request(std::string buffer, HttpRequest &req)
{
    std::istringstream stream(buffer);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.find("GET") != std::string::npos)
            return (parse_request("GET", buffer, req));
        else if (line.find("POST") != std::string::npos)
            return (parse_request("POST", buffer, req));
        else if (line.find("DELETE") != std::string::npos)
            return (parse_request("DELETE", buffer, req));
        // DEBUG DE TFREYDIE A SUPPR APRES
        else if (line.find("CGI") != std::string::npos)
            return ("CGI");
    }
    return ("");
}