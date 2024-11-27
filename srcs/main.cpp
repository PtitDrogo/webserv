#include "Webserv.hpp"

bool server_running = true; //we can hide this variable in a class statically somewhere
static void handleSignal(int signum);

int main(int argc, char **argv, char **envp)
{
	Config conf;
	HttpRequest req;
	std::vector<struct pollfd> fds;
	(void) envp;

	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return 0;
	}
	if (conf.parse_config_file(argv[1]) == false)
		std::cout << "Webserv : [WARNING] conflicting server name" << std::endl;
	size_t number_of_servers = conf.addAllServers(fds);
	std::cout << "Did I add 3 servers, current server count : " << number_of_servers << std::endl;

	while (server_running)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
		checkIfNewClient(fds, number_of_servers, conf);
		if (safe_poll(fds, number_of_servers) == FAILURE)
			return FAILURE;
		for (size_t i = number_of_servers; i < fds.size(); ++i) //honestly this is to the point
		{
			Client &client = conf.getClientObject(fds[i].fd);
			if (fds[i].revents & POLLRDHUP)
			{
				printf("disconnect client of main loop\n");
				disconnectClient(fds, i, conf);
				break;
			}
			if (!(fds[i].revents & POLLIN))
				continue;
			// Lecture initiale du buffer
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (handleRecvValue(recv_value, i, fds, conf) == FAILURE)
				break ;

			std::string type_request = get_type_request(buffer, req);
			// parse_allow_method(req, conf, fds[i].fd);
			std::cout << "TYPE REQUEST IS : " << type_request << std::endl; 
			if (type_request == "POST")
			{
				if (preparePostParse(client, buffer,recv_value) == false)
					break ;
			}
			else if (type_request == "GET")
				parse_buffer_get(client, buffer,req);
			else if (type_request == "DELETE")
				parse_buffer_delete(buffer, client);
			else if (type_request == "CGI")
				cgiProtocol(envp, req, fds[i].fd);
			else
				generate_html_page_error(client, "404");
			std::cout << req << std::endl;
		}
	}
	return SUCCESS;
}

static void handleSignal(int signum) {
    static_cast<void>(signum);
    std::cout << "server shutdown" << std::endl;
    server_running = false;
    signal(SIGINT, &handleSignal);
    signal(SIGTERM, &handleSignal);
}


