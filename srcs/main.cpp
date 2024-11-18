#include "include.hpp"
#include "Webserv.hpp"
#include "config.hpp"
#include "httpRequest.hpp"

std::string parse_request(std::string type, std::string buffer, HttpRequest &req)
{
	std::istringstream stream(buffer);
	std::string line;

	std::string method;
	std::string path;
	std::string version;
	std::string body;
	if (type == "GET")
	{
		while (std::getline(stream, line))
		{
			size_t pos1 = line.find("GET");
			size_t pos2 = line.find("HTTP");
		
			if (pos1 != std::string::npos && pos2 != std::string::npos)
			{
				method = line.substr(pos1, 4);
				req.setMetode(method);
				path = line.substr(pos1 + 4, pos2 - pos1 - 5);
				req.setPath(path);
				version = line.substr(pos2);
				req.setVersion(version);
			}
			body += line + "\n";
		}
		req.setBody(body);
		return ("GET");
	}
	if (type == "POST")
	{
		while (std::getline(stream, line))
		{
			size_t pos1 = line.find("POST");
			size_t pos2 = line.find("HTTP");
		
			if (pos1 != std::string::npos && pos2 != std::string::npos)
			{
				method = line.substr(pos1, 5);
				req.setMetode(method);
				path = line.substr(pos1 + 5, pos2 - pos1 - 6);
				req.setPath(path);
				version = line.substr(pos2);
				req.setVersion(version);
			}
			body += line + "\n";
		}
		req.setBody(body);
		return ("POST");
	}
	if (type == "DELETE")
	{
		while (std::getline(stream, line))
		{
			size_t pos1 = line.find("DELETE");
			size_t pos2 = line.find("HTTP");
		
			if (pos1 != std::string::npos && pos2 != std::string::npos)
			{
				method = line.substr(pos1, 7);
				req.setMetode(method);
				path = line.substr(pos1 + 7, pos2 - pos1 - 8);
				req.setPath(path);
				version = line.substr(pos2);
				req.setVersion(version);
			}
			body += line + "\n";
		}
		req.setBody(body);
		return ("DELETE");
	}
	return ("");
}

std::string get_type_request(std::string buffer, HttpRequest &req)
{
	std::istringstream stream(buffer);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.find("GET") != std::string::npos)
			return (parse_request("GET", buffer, req));
		if (line.find("POST") != std::string::npos)
			return (parse_request("POST", buffer, req));
		if (line.find("DELETE"))
			return (parse_request("DELETE", buffer, req));
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

// int main(int argc, char **argv)
// {
// 	Config conf;
// 	Server serv;
// 	// location loc;

// 	if (argc != 2)
// 	{
// 		std::cout << "error : use ./webserv file.conf" << std::endl;
// 		return (0);
// 	}
// 	// conf.parse_config_file(serv, loc, argv[0]);
// 	conf.parse_config_file(serv, argv[1]);
// 	// printVectorloc2(serv.getLocation());
// 	// printVectorServer2(conf.getServer());

// 	int server_socket = SetupSocket(serv, conf);
	
// 	// Prepare the pollfd structure and add the server_poll
// 	std::vector<struct pollfd> fds; 
// 	addPollFD(server_socket, fds);
// 	while (true)
// 	{
// 		checkIfNewClient(fds, server_socket);
// 		if (safe_poll(fds, server_socket) == FAILURE)
// 			return FAILURE;
// 		for (size_t i = 1; i < fds.size(); ++i)
// 		{
// 			if (fds[i].revents & POLLRDHUP)
// 			{
// 				disconnectClient(fds, i);
// 				break ;
// 			}
// 			if (!(fds[i].revents & POLLIN))
// 				continue;
// 			char buffer[10000] = {0};
// 			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0); //MSG_DONTWAIT in case of trouble
// 			if (handleRecvValue(recv_value, i, fds) == FAILURE)
// 				break ;
// 			// printf("DEBUG: My request is %s\n", buffer);
// 			std::string type_request = get_type_request(buffer);
// 			if (type_request == "GET")
// 				parse_buffer_get(buffer, conf, fds[i].fd);
// 			if (type_request == "POST")
// 				parse_buffer_post(buffer , fds[i].fd, conf);
// 			if (type_request == ("DELETE"))
// 				parse_buffer_delete(buffer, fds[i].fd, conf);
// 			else
// 				generate_html_page_error(conf, fds[i].fd, "404");
// 			std::cout << buffer << std::endl;
// 		}
// 	}
// 	return (SUCCESS);
// }


int main(int argc, char **argv)
{
	Config conf;
	Server serv;
	HttpRequest req;

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
			if (type_request == "POST")
			{
				std::string initial_data(buffer, recv_value);
				size_t content_length_pos = initial_data.find("Content-Length: ");
				if (content_length_pos == std::string::npos)
				{
					generate_html_page_error(conf, fds[i].fd, "400");
					break;
				}

				size_t length_start = content_length_pos + 16;
				size_t length_end = initial_data.find("\r\n", length_start);
				int content_length = 0;
				std::istringstream(initial_data.substr(length_start, length_end - length_start)) >> content_length;

				int content_length_size_t = content_length;
				if (content_length_size_t > conf.getServer()[0].getMaxBodySize())
				{
					generate_html_page_error(conf, fds[i].fd, "400");
					break;
				}

				std::string body = initial_data;
				int total_received = body.size();
				while (total_received < content_length_size_t)
				{
					recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (recv_value <= 0)
					{
						std::cerr << "Erreur : données POST incomplètes." << std::endl;
						generate_html_page_error(conf, fds[i].fd, "400");
						break;
					}

					body.append(buffer, recv_value);
					total_received += recv_value;
				}
				parse_buffer_post(body, fds[i].fd, conf);
			}
			else if (type_request == "GET")
				parse_buffer_get(buffer, conf, fds[i].fd);
			else if (type_request == "DELETE")
				parse_buffer_delete(buffer, fds[i].fd, conf);
			else
				generate_html_page_error(conf, fds[i].fd, "404");
			std::cout << "-------REQUEST------" << std::endl;
			std::cout << "methode = " << req.getMetode() << std::endl;
			std::cout << "path = " << req.getPath() << std::endl;
			std::cout << " version = " << req.getVersion() << std::endl;
			std::cout << " body = " << req.getBody() << std::endl;
		}
	}
	return SUCCESS;
}


