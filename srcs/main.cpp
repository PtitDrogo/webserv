#include "Webserv.hpp"

bool server_running = true; //we can hide this variable in a class statically somewhere
static void handleSignal(int signum);
// static bool isNOTCgiStuff(Client& client, Config &conf, std::vector<struct pollfd> &fds, size_t i);

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
			std::cout << "In client index" << i << "revents is : " << fds[i].revents << std::endl;
			std::cout << "fds.size() is : " << fds.size() << std::endl;
			Client &client = conf.getClientObject(fds[i].fd);
			if (fds[i].revents & POLLRDHUP)
			{
				printf("disconnect client of main loop\n");
				disconnectClient(fds, i, conf);
				continue;
			}
			// isNOTCgiStuff(req, client, conf, fds, i); //TFREYDIE CGI STUFF WORK IN PROGRESS
			if (!(fds[i].revents & POLLIN) || client.getCgiCaller() != NULL) //that means its a pipe
				continue;
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


// static bool isNOTCgiStuff(Client& client, Config &conf, std::vector<struct pollfd> &fds, size_t i)
// {
// 	printf("Caller of current client is : %p, fds[i].revents is %i\n", client.getCgiCaller(), fds[i].revents);
// 	if (client.getCgiCaller() != NULL && fds[i].revents & POLLIN)
// 	{
// 		printf("Pipe disconnected1\n");
// 		//I want my client caller to send the content from the cgi pipe to its websocket;
// 		//then we disconnect client of Pipe and all is well;

// 		std::string cgi_output = readFromPipeFd(fds[i].fd);
// 		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
// 		send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0);
// 		waitpid(-1, 0, 0); // Collect the child process ressources;
// 		disconnectClient(fds, i, conf);
// 		return false;
// 		// wait;
// 	}
// 	if (client.getCgiCaller() != NULL && fds[i].revents & POLLHUP)
// 	{
// 		printf("Pipe disconnected2\n");
// 		//I want my client caller to send the content from the cgi pipe to its websocket;
// 		//then we disconnect client of Pipe and all is well;

// 		std::string cgi_output = readFromPipeFd(fds[i].fd);
// 		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
// 		send(client.getSocket(), response.c_str(), response.size(), 0);
// 		waitpid(-1, 0, 0); // Collect the child process ressources;
// 		disconnectClient(fds, i, conf);
// 		return false;
// 		// wait;
// 	}
// 	return true;
// }

