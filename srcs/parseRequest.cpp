#include "Webserv.hpp"

static bool isCgiDelim(char c);

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
    //Added the check for cgi here
    if (isCgiRequest(req) == true)
    {    
        req.setMethod("CGI");
        std::cout << "This shit is a CGI nocap nocap" << std::endl;
        return ("CGI");
    }
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
    }
    return ("");
}

//I think this works as intended
bool isCgiRequest(const HttpRequest &req)
{
    const char *valid_cgis[] = {".py", ".sh", ".php"}; //We can always add more here
    unsigned int cgis_list_len = sizeof(valid_cgis) / sizeof(char *);

    std::string path = req.getPath();
    unsigned int i;

    std::cout << RED << "In isCgiReQuest" << RESET << std::endl;
    // std::cout << "|" << path << "|" << std::endl;
    
    
    for (i = 0; i < path.size(); i++)
    {
        std::cout << path[i] << std::endl;
        if (isCgiDelim(path[i]) == true)
            break ;
    }
    //Smallest programm possible is x.py
    if (i < 4)
        return false;
    // size_t pos_of_questionmark = path.find("?");
    std::string check_3 = path.substr(i - 3, 3);
    std::string check_4 = path.substr(i - 4, 4);

    std::cout << "i is : " << i << " ,check 3 and check 4 are : " << check_3 << ", " << check_4 << std::endl;

    for (unsigned int j = 0; j < cgis_list_len; j++)
    {
        if (check_3 == valid_cgis[j] || check_4 == valid_cgis[j])
            return (true);
    }
    return (false);
}

static bool isCgiDelim(char c)
{
    return (std::isspace(c) || c == '?');
}