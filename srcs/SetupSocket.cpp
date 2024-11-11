/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SetupSocket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:21:30 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/11 18:21:32 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/server.hpp"

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