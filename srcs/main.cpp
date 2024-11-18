#include "Webserv.hpp"

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
				generate_html_page_error(conf, fds[i].fd, "404");
			}
			std::cout << req << std::endl;
		}
	}
	return SUCCESS;
}


