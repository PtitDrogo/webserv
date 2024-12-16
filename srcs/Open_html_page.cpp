#include "Webserv.hpp"
#include "config.hpp"
#include "server.hpp"
#include <fstream>
#include "sys/stat.h"


std::string readFile(std::string &path)
{
    // std::cout << "DEBUG: path = " << path << std::endl;

    // if (!isRegularFile(path))
    // {
    //     std::cerr << "Error: " << path << " is not a regular file." << std::endl;
    //     return "";
    // }

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file" << std::endl;
        return "";
    }
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}


std::string httpHeaderResponse(std::string code, std::string contentType, std::string content)
{
	return ("HTTP/1.1 " + code + "\r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + toString(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content); //IT REALLY SHOULD BE CLOSE OR KEEP ALIVE DEPENDING ON CONTEXT;
}

void generate_default_error_page(std::string error_code, int client_socket, const Client& client)
{
    std::string path;
    if (error_code == "400")
        path = "." + client.getServer().getRoot() + "default_error_pages/400.html";
    else if (error_code == "403")
        path = "." + client.getServer().getRoot() + "default_error_pages/403.html";
    else if (error_code == "404")
        path = "." + client.getServer().getRoot() + "default_error_pages/404.html";
    else if (error_code == "413")
        path = "." + client.getServer().getRoot() + "default_error_pages/413.html";
    else
        path = "." + client.getServer().getRoot() + "default_error_pages/404.html";
    std::string file_content = readFile(path);
    std::string reponse = httpHeaderResponse(error_code, "text/html", file_content);
    send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void generate_html_page_error(const Client& client, std::string error_code)
{
    const Server& server = client.getServer();

    std::map<std::string, std::string> errorPageMap = server.getErrorPage();

    std::map<std::string, std::string>::iterator it = errorPageMap.find(error_code);
    std::string path;
    if (it != errorPageMap.end())
    {
        path = "." + it->second;
    }
    else
    {
        std::cerr << "Error page for code " << error_code << " not found." << std::endl;
        generate_default_error_page(error_code, client.getSocket(), client);
        return;
    }
    std::string file_content = readFile(path);
    std::string response = httpHeaderResponse(error_code, "text/html", file_content);

     std::cout << GREEN << response.c_str() << RESET << std::endl;
    send(client.getSocket(), response.c_str(), response.size(), 0); //TODO CHECK ALL SEND
}


