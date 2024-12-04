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

	while (server_running)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
		checkIfNewClient(fds, number_of_servers, conf);
		if (safe_poll(fds, number_of_servers) == FAILURE)
			return FAILURE;

		std::cout << "je suis la -----------------------------------------------------------------" << std::endl;
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
			// parse_allow_method(req, conf, fds[i].fd);
			std::cout << "TYPE REQUEST IS : " << type_request << std::endl; 
			if (type_request == "POST")
			{
				int	server_index = conf.getIndexOfClientServer(fds[i].fd);
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

				if (content_length > conf.getServer()[server_index].getMaxBodySize())
				{
					generate_html_page_error(conf, fds[i].fd, "413");
					break;
				}

				// Extraire le corps après la ligne vide qui suit les en-têtes
				std::string body = initial_data.substr(initial_data.find("\r\n\r\n") + 4);

				// Afficher le contenu reçu pour déboguer
				std::cout << "initial_data = " << initial_data << std::endl;
				std::cout << "body = " << body << std::endl;

				int total_received = body.size();
				while (total_received < content_length)
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
				parse_buffer_get(buffer, conf, fds[i].fd, req);
			else if (type_request == "DELETE")
				parse_buffer_delete(buffer, fds[i].fd, conf);
			else if (type_request == "CGI")
			{
				cgiProtocol(envp, req, fds[i].fd);
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


