/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/07 15:12:53 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;
	
	Config conf;

	// recupere les ports
	conf.parse_config_file(); 

	// creation du socket //
	int server_socket = socket(AF_INET, SOCK_STREAM, 0); 

	// creation de l'adresse du serveur //
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(conf.get_ports()[0]);
	server_address.sin_addr.s_addr = INADDR_ANY;

	std::cout << "Server socket created" << std::endl;
	std::cout << "Server port: " << ntohs(server_address.sin_port) << std::endl;

	// liaison du socket //
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Erreur lors de la liaison du socket." << std::endl;
		close(server_socket);
		return -1;
	}

	std::cout << "Server socket binded" << std::endl;
	std::cout << "Server listening on port " << ntohs(server_address.sin_port) << std::endl;

	// mise en écoute du serveur //
	if (listen(server_socket, 5) == -1)
	{
		std::cerr << "Erreur lors de la mise en écoute." << std::endl;
		close(server_socket);
		return -1;
	}
	
	std::cout << "Server listening" << std::endl;

	// acceptation de la connexion //
	while (true)
	{
		sockaddr_in client_address;
		socklen_t client_size = sizeof(client_address);
		int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
		if (client_socket == -1)
		{
			std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
			close(server_socket);
			continue ;
		}
		std::cout << "Connexion acceptée !" << std::endl;
		std::ifstream html_file("./config/web.html");
		if (!html_file.is_open())
		{
			std::cerr << "Erreur lors de l'ouverture du fichier HTML." << std::endl;
			close(client_socket);
			close(server_socket);
			continue ;
		}
		std::stringstream buffer;
		buffer << html_file.rdbuf();
		std::string html_content = buffer.str();
		html_file.close();
		size_t content_length = html_content.size();
		char response_header[1024];
		snprintf(response_header, sizeof(response_header),
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: %zu\r\n\r\n", content_length);
		send(client_socket, response_header, strlen(response_header), 0);
		send(client_socket, html_content.c_str(), html_content.size(), 0);
		std::cout << "Réponse envoyée au client." << std::endl;
		close(client_socket);
	}
	return 0;
}
