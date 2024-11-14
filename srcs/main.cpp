#include "include.hpp"
#include "Webserv.hpp"
#include "config.hpp"

std::string get_type_request(std::string buffer)
{
	std::istringstream stream(buffer);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.find("GET") != std::string::npos)
			return ("GET");
		if (line.find("POST") != std::string::npos)
			return ("POST");
		if (line.find("DELETE"))
			return ("DELETE");
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

int main(int argc, char **argv)
{
	Config conf;
	Server serv;
	// location loc;

	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return (0);
	}
	// conf.parse_config_file(serv, loc, argv[0]);
	conf.parse_config_file(serv, argv[1]);
	// printVectorloc2(serv.getLocation());
	// printVectorServer2(conf.getServer());

	int server_socket = SetupSocket(serv, conf);
	
	// Prepare the pollfd structure and add the server_poll
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
				break ;
			}
			if (!(fds[i].revents & POLLIN))
				continue;
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0); //MSG_DONTWAIT in case of trouble
			if (handleRecvValue(recv_value, i, fds) == FAILURE)
				break ;
			// printf("DEBUG: My request is %s\n", buffer);
			std::string type_request = get_type_request(buffer);
			if (type_request == "GET")
				parse_buffer_get(buffer, conf, fds[i].fd);
			if (type_request == "POST")
				parse_buffer_post(buffer , fds[i].fd, conf);
			if (type_request == ("DELETE"))
				parse_buffer_delete(buffer, fds[i].fd, conf);
			else
				generate_html_page_error(conf, fds[i].fd, "404");
			std::cout << buffer << std::endl;
		}
	}
	return (SUCCESS);
}

