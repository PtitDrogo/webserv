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
	conf.parse_config_file(argv[1]);
	size_t number_of_servers = conf.addAllServers(fds);
	while (server_running)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
		checkIfNewClient(fds, number_of_servers, conf);
		if (safe_poll(fds, number_of_servers) == FAILURE)
			return FAILURE;
		for (size_t i = number_of_servers; i < fds.size(); ++i)
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
				cgiProtocol(envp, req); //obviously temporary
				std::string tmp = fileToString(PATH_CGI_OUT);
				send(fds[i].fd, tmp.c_str(), tmp.size(), 0);
			}
			else
			{	
				generate_html_page_error(conf, fds[i].fd, "404");
			}
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


