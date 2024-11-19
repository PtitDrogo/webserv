#include "Webserv.hpp"

bool server_running = true;
static void handleSignal(int signum);

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

	// Rajouter les Serveurs Sur la liste des fds
	// Je fout le start de i au nombre de serveur
	// Puis du coup tout les serveurs rajoute les sockets client sur le meme truc en soit c'est des serveurs VIRTUELS !

	//Je dois etre capable de rajouter plusieurs serveurs.
	//La socket client doit avoir l'info quelquepart de quel serveur l'a creer.

	while (server_running)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
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


