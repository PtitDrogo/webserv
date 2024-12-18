#include "Webserv.hpp"
#include "config.hpp"
#include "server.hpp"
#include <fstream>
#include "sys/stat.h"


std::string readFile(std::string &path)
{
	if (!isRegularFile(path))
	{
		return "";
	}

	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
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
			"\r\n" + content);
}

bool generate_default_error_page(std::string error_code, int client_socket, const Client& client)
{
	std::string path;

	if (error_code == "400")
		path = "." + client.getServer().getRoot() + "default_error_page/400.html";
	else if (error_code == "403")
		path = "." + client.getServer().getRoot() + "default_error_page/403.html";
	else if (error_code == "404")
		path = "." + client.getServer().getRoot() + "default_error_page/404.html";
	else if (error_code == "413")
		path = "." + client.getServer().getRoot() + "default_error_page/413.html";
	else
		path = "." + client.getServer().getRoot() + "default_error_page/404.html";
	std::string file_content = readFile(path);
	std::string reponse = httpHeaderResponse(error_code, "text/html", file_content);
	if (send(client_socket, reponse.c_str(), reponse.size(), 0) == -1)
		return false;
	return true;
}

bool generate_html_page_error(const Client& client, std::string error_code)
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
		bool status = generate_default_error_page(error_code, client.getSocket(), client);
		return status;
	}
	std::string file_content = readFile(path);
	std::string response = httpHeaderResponse(error_code, "text/html", file_content);
	std::cout << GREEN << response.c_str() << RESET << std::endl;
	if (send(client.getSocket(), response.c_str(), response.size(), 0) == -1)
		return false;
	return true;
}


