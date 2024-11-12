/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 16:51:40 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/Setup_socket.hpp"
#include "../include/Open_html_page.hpp"
#include <poll.h> 

int parse_buffer(std::string buffer, location &loc)
{
	std::istringstream stream(buffer);
	std::string line;

	if (!stream)
	{
		std::cout << "Erreur : le flux n'a pas pu être créé." << std::endl;
		return false;
	}
	std::string method;
	std::string path;
	std::string version;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("GET");
		size_t pos2 = line.find("HTTP");
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			method = line.substr(pos1, 4);
			path = line.substr(pos1 + 4, pos2 - pos1 - 4);
			version = line.substr(pos2);
			if (path == loc.getPath())
				return 1; // ouvrir la page avec location
			if (path[0] == '/' && path[1] == ' ')
				return 2; // ouvrir la page sans location
			return 0;
		}
	}
	return 0;
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
		// printf("hi1\n");
		// printf("hi2\n");
		if (fds[0].revents & POLLIN)
        {
			// printf("hiX\n");
			int client_socket = SetupClientAddress(server_socket);
			addPollFD(client_socket, fds);
			printf("DEBUG: Added client to the list\n");
		}
		if (poll(fds.data(), fds.size(), -1) == -1)
        {
            std::cerr << "Poll failed" << std::endl;
            close(server_socket); //technically would have to do more cleanup than that;
            return 1;
        }
		// printf("hi3\n");
		for (size_t i = 1; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLRDHUP)
			{
				printf("If you see me you win\n");
				disconnectClient(fds, i);
				break ;
			}
			if (!(fds[i].revents & POLLIN))
                continue; //beautiful, if nothing happens, just skip it
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0); //MSG_DONTWAIT in case of trouble
			if (handleRecvValue(recv_value, i, fds) == 1)
				break ; //For now maybe idk
			printf("My request is %s\n", buffer);
			if (parse_buffer(buffer, loc) == 0)
				generate_html_page404(serv, fds[i].fd);
			if (parse_buffer(buffer, loc) == 2)
				generate_html_page_without_location(serv, fds[i].fd);
			else if (parse_buffer(buffer, loc) == 1)
				generate_html_page_with_location(serv, loc, fds[i].fd);
			// printf("hi5\n");
		}
	}
	return (0);
}

