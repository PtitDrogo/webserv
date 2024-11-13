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
	}
	return ("");
}


int main(int argc, char **argv)
{
	Config conf;
	Server serv;
	location loc;

	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return (0);
	}
	conf.parse_config_file(serv, loc, argv[1]);
	int server_socket = SetupSocket(serv);
	
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
				parse_buffer_get(buffer, serv, fds[i].fd);
			if (type_request == "POST")
				parse_buffer_post(buffer , fds[i].fd, serv);
			else
				generate_html_page_error(serv, fds[i].fd, "404");
			std::cout << buffer << std::endl;
		}
	}
	return (SUCCESS);
}

