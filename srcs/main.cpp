#include "Webserv.hpp"

// void parse_allow_method(HttpRequest &req, Config &conf, int client_socket)
// {
// 	std::vector<std::string> methods;

//     // Utiliser std::istringstream pour découper la chaîne
//     std::istringstream stream(conf.getServer()[0].getLocation()[0].getAllowMethod());
//     std::string method;

//     while (stream >> method) {
//         methods.push_back(method); // Ajouter chaque méthode dans le vecteur
//     }

// 	// Vérifier si la méthode de la requête est autorisée
// 	if (std::find(methods.begin(), methods.end(), req.getMetode()) == methods.end()) {
// 		generate_html_page_error(conf, client_socket, "405");
// 	}
// }


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
	if (conf.parse_config_file(serv, argv[1]) == false)
		std::cout << "Webserv : [WARNING] conflicting server name" << std::endl;

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
			// parse_allow_method(req, conf, fds[i].fd);
			std::cout << "TYPE REQUEST IS : " << type_request << std::endl; 
			if (type_request == "POST")
			{
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

				if (content_length > conf.getServer()[0].getMaxBodySize())
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
				cgiProtocol(envp, "time.py"); //obviously temporary
				std::string tmp = fileToString(PATH_CGI_OUT);
				send(fds[i].fd, tmp.c_str(), tmp.size(), 0);
			}
			else
			{	
				std::cout << "hi" << std::endl;
				generate_html_page_error(conf, fds[i].fd, "404");
			}
			std::cout << "-------REQUEST------" << std::endl;
			std::cout << "methode = " << req.getMethod() << std::endl;
			std::cout << "path = " << req.getPath() << std::endl;
			std::cout << " version = " << req.getVersion() << std::endl;
			std::cout << " body = " << req.getBody() << std::endl;
		}
	}
	return SUCCESS;
}


