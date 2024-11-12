/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SetupSocket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:21:30 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 16:44:04 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/server.hpp"
#include "poll.h"

int SetupSocket(Server serv)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Erreur lors de la configuration du socket." << std::endl;
		close(server_socket);
		return -1;
	}
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	int port = std::atoi(serv.getPort().c_str());
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Erreur lors de la liaison du socket." << std::endl;
		close(server_socket);	
		return -1;
	}
	if (listen(server_socket, 5) == -1)
	{
		std::cerr << "Erreur lors de la mise en écoute." << std::endl;
		close(server_socket);
		return -1;
	}
	return server_socket;
}

int SetupClientAddress(int server_socket)
{
	///////////////////////////////
	/////////ACCEPT////////////////
	///////////////////////////////
	sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);
	int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
	if (client_socket == -1)
	{
		std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
		close(server_socket);
		return -1;
	}
	return client_socket;
}

void disconnectClient(std::vector<struct pollfd> &fds, size_t &i)
{
	std::cout << "Client disconnected" << std::endl;
	close(fds[i].fd);
	fds.erase(fds.begin() + i);
	--i;
}



int	handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds) //add the list of pollfds later;
{

	if (valread > 0)
	{
		std::cout << "DEBUG:Received from client successfully" << std::endl;
		return (0);
	}
	else if (valread == 0)
	{
		
		std::cout << "DEBUG:Recve detected no client, disconnecting" << std::endl;
		disconnectClient(fds, i);
		return (1); //In theory this should never trigger but leaving just in case
	}
	else
	{
		// Error reading from the client
		// if (errno == EAGAIN || errno == EWOULDBLOCK) //this means no errors happened
		// {
		// 	errno = 0;
		// 	return 2; //kill me
		// }
		std::cerr << "Error reading from client" << std::endl;
		disconnectClient(fds, i);
		return (1);
	}
}

void addPollFD(int client_socket, std::vector<struct pollfd> &fds)
{
	if (client_socket != -1)
	{
		struct pollfd client_pollfd;
		client_pollfd.fd = client_socket;
		client_pollfd.events = POLLIN | POLLRDHUP; //This can be several things;
		client_pollfd.revents = 0;
		fds.push_back(client_pollfd);
	}
	return ;
}

