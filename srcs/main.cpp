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
			std::cout << "number of servers is : " << number_of_servers << std::endl;
			std::cout << "In client index : " << i << ", revents is : " << fds[i].revents << std::endl;
			std::cout << "fds.size() is : " << fds.size() << std::endl;
			if (fds[i].revents & POLLRDHUP)
			{
				printf("disconnect client of main loop\n");
				disconnectClient(fds, i, conf);
				continue;
			}
			Client &client = conf.getClientObject(fds[i].fd); //I need client first to know if it timeouted;
			if (client.didClientTimeout() == true && client.getCgiCaller() != NULL)
			{
				generate_html_page_error(client, "504");
				disconnectClient(fds, i, conf);
				continue;
			}
			if ((!(fds[i].revents & POLLIN))) // || (!(fds[i].revents & POLLOUT)) maybe later but rn its infinite
				continue;
			if (isCgiStuff(client, conf, fds, i) == true)
				continue ; //TFREYDIE CGI STUFF WORK IN PROGRESS
			std::cout << "ALLO" << std::endl;
			// Lecture initiale du buffer
			char buffer[4096] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (handleRecvValue(recv_value, i, fds, conf) == FAILURE)
				break ;
			
			// on ajoute le buffer à la requete + recuperation du content-length et on update le totalRead
			client.appendToRequest(buffer, recv_value);

			// si on a recu toute la requete
			if (client.getTotalRead() >= client.getContentLength()) {
				std::cout << MAGENTA << "Full request received" << RESET << std::endl;	// debug
				std::cout << GREEN << client.getRequest() << RESET << std::endl;		// debug request

				std::string type_request = get_type_request(client.getRequest(), req);
				std::cout << BLUE << "TYPE REQUEST IS : " << type_request << RESET << std::endl; 
				
				if (type_request == "POST")
				{
					if (preparePostParse(client, client.getRequest()) == false)
						break ;
				}
				else if (type_request == "GET")
					parse_buffer_get(client.getRequest(), conf, client, req);
				else if (type_request == "DELETE")
					parse_buffer_delete(client.getRequest(), client);
				else if (type_request == "CGI")
					cgiProtocol(envp, req, client, conf, fds);
				else
					generate_html_page_error(client, "404");
				// std::cout << req << std::endl;
			}
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



