#include "Webserv.hpp"
#include "Cookies.hpp"


static void handleSignal(int signum);

int main(int argc, char **argv, char **envp)
{
	Config conf;
	// HttpRequest req;
	Cookies cook;
	std::vector<struct pollfd> fds;

	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return 0;
	}
	if (conf.parse_config_file(argv[1]) == false)
	{
		return 0;
	}
	size_t number_of_servers = conf.addAllServers(fds);

	while (Config::ServerRunning)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
		checkIfNewClient(fds, number_of_servers, conf);
		if (safe_poll(fds, number_of_servers) == FAILURE)
			return FAILURE;
		for (size_t i = number_of_servers; i < fds.size(); ++i) //honestly this is to the point
		{
			Client &client = conf.getClientObject(fds[i].fd);
			HttpRequest req;
			if (fds[i].revents & POLLRDHUP || fds[i].revents & POLLHUP)
			{
				// printf("disconnect client of main loop, disconnected client %i\n", fds[i].fd);
				disconnectClient(fds, client, conf);
				break;
			}
			if (fds[i].revents & POLLERR)
			{
				printf("error with this client, were killing it, disconnected client %i\n", fds[i].fd);
				disconnectClient(fds, client, conf);
				break;
			}
			if (handleTimeout(client, fds, conf, i) == true)
				continue ;
			if ((!(fds[i].revents & POLLIN)))
				continue;
			if (isCgiStuff(client, conf, fds, i) == true)
				continue ;
			// Lecture initiale du buffer
			char buffer[4096] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (handleRecvValue(recv_value) == FAILURE)
			{	
				disconnectClient(fds, client, conf);
				break ;
			}
			// on ajoute le buffer à la requete + recuperation du content-length et on update le totalRead
			client.appendToRequest(buffer, recv_value);

			// si on a recu toute la requete
			if (client.getTotalRead() >= client.getContentLength()) {
				// std::cout << MAGENTA << "Full request received" << RESET << std::endl;	// debug
				// std::cout << GREEN << client.getRequest() << RESET << std::endl;		// debug request
				// std::cout << "DEBUG , path in main is : " << client.

				std::string type_request = get_type_request(client.getRequest(), req);
				std::cout << BLUE << "TYPE REQUEST IS : " << type_request << RESET << std::endl; 
				if (type_request == "POST")
				{
					if (preparePostParse(client, cook, req) == false)
						break ;
				}
				else if (type_request == "GET") 
				{
					if (prepareGetParse(client, cook, req) == false) 
						break ;
				}
				else if (type_request == "DELETE")
					parse_buffer_delete(client.getRequest(), client);
				else if (type_request == "CGI-GET" || type_request == "CGI-POST")
					cgiProtocol(envp, req, client, conf, fds);
				else
					generate_html_page_error(client, "400");
				client.reset();
				std::cout << req << std::endl;
			}
		}
	}
	return SUCCESS;
}

static void handleSignal(int signum) 
{
    static_cast<void>(signum);
    std::cout  << std::endl << MAGENTA << "Shutting Down Server, Bye !"  << RESET << std::endl;
    Config::ServerRunning = false;
    signal(SIGINT, &handleSignal);
    signal(SIGTERM, &handleSignal);
}



