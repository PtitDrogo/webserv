/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/13 15:17:09 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "Webserv.hpp"
#include "config.hpp"

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
		checkIfNewClient(fds, server_socket);
		if (safe_poll(fds, server_socket) == FAILURE)
            return FAILURE;
		for (size_t i = 1; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLRDHUP)
			{
				disconnectClient(fds, i);
				break ;
			}
			if (!(fds[i].revents & POLLIN))
                continue;
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0); //MSG_DONTWAIT in case of trouble
			if (handleRecvValue(recv_value, i, fds) == FAILURE)
				break ;
			// printf("DEBUG: My request is %s\n", buffer);
			generateResponse(serv, buffer, loc, fds[i].fd);
		}
	}
	return (SUCCESS);
}

