/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 17:26:11 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/Setup_socket.hpp"
#include "../include/Open_html_page.hpp"
#include "../include/parseBuffer.hpp"
#include <poll.h>

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
		if (fds[0].revents & POLLIN)
        {
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
		for (size_t i = 1; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLRDHUP)
			{
				disconnectClient(fds, i);
				break ;
			}
			if (!(fds[i].revents & POLLIN))
                continue; //beautiful, if nothing happens, just skip it
			char buffer[1024] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0); //MSG_DONTWAIT in case of trouble
			if (handleRecvValue(recv_value, i, fds) == 1)
				break ;
			printf("DEBUG: My request is %s\n", buffer);
			generateResponse(serv, buffer, loc, fds[i].fd);
		}
	}
	return (0);
}

