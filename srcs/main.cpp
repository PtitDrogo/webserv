#include "include.hpp"
#include "Webserv.hpp"
#include "config.hpp"
#include "CgiHandler.hpp"
#include "httpRequest.hpp"


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
            req.setMetode(method);
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
		//DEBUG DE TFREYDIE A SUPPR APRES
		else if (line.find("CGI") != std::string::npos)
			return ("CGI");
	}
	return ("");
}

void printVectorloc2(std::vector<location> loc)
{
	for (size_t i = 0; i < loc.size(); i++)
	{
		std::cout << "Path: " << loc[i].getPath() << std::endl;
		std::cout << "Index: " << loc[i].getIndex() << std::endl;
	}
}

// void printVectorServer2(std::vector<Server> serv)
// {
// 	for (size_t i = 0; i < serv.size(); i++)
// 	{
// 		std::cout << "server { " << std::endl;
// 		std::cout << "Port: " << serv[i].getPort() << std::endl;
// 		std::cout << "Server Name: " << serv[i].getServerName() << std::endl;
// 		std::cout << "Root: " << serv[i].getRoot() << std::endl;
// 		std::cout << "Index: " << serv[i].getIndex() << std::endl;
// 		std::cout << "Error Page: " << serv[i].getErrorPage("403") << std::endl;
// 		printVectorloc2(serv[i].getLocation());
// 		std::cout << "}" << std::endl;
// 	}
// }

int main(int argc, char **argv,char **envp)
{
	Config conf;
	Server serv;
	HttpRequest req;
	(void) envp;


	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return 0;
	}
	conf.parse_config_file(serv, argv[1]);

	int server_socket = SetupSocket(serv, conf);

	// Préparer la structure pollfd
	std::vector<struct pollfd> fds;
	addPollFD(server_socket, fds);

	while (true)
	{
		checkIfNewClient(fds, server_socket);
		if (safe_poll(fds, server_socket) == FAILURE)
			return FAILURE;

		for (size_t i = 1; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLRDHUP)
			{
				disconnectClient(fds, i);
				break;
			}
			if (!(fds[i].revents & POLLIN))
				continue;

			// Lecture initiale du buffer
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (handleRecvValue(recv_value, i, fds) == FAILURE)
				break;

			std::string type_request = get_type_request(buffer, req);
			std::cout << "TYPE REQUEST IS : " << type_request << std::endl; 
			if (type_request == "POST")
			{
				if (!preparePostParse(fds[i].fd, buffer, conf, recv_value))
					break;
			}
			if (type_request == "GET")
				parse_buffer_get(buffer, conf, fds[i].fd);
			else if (type_request == "DELETE")
				parse_buffer_delete(buffer, fds[i].fd, conf);
			else if (type_request == "CGI")
			{
				cgiProtocol(envp, "time.py"); //obviously temporary
				std::string tmp = fileToString(PATH_CGI_OUT);
				send(fds[i].fd, tmp.c_str(), tmp.size(), 0);
			}
			else
			{	
				std::cout << "hi" << std::endl;
				generate_html_page_error(conf, fds[i].fd, "404");
			}
			std::cout << "-------REQUEST------" << std::endl;
			std::cout << "methode = " << req.getMetode() << std::endl;
			std::cout << "path = " << req.getPath() << std::endl;
			std::cout << " version = " << req.getVersion() << std::endl;
			std::cout << " body = " << req.getBody() << std::endl;
		}
	}
	return SUCCESS;
}


