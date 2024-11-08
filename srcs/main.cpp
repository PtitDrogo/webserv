/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/08 16:37:43 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


std::string readFile(std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cout << path << ": ";
		std::cout << "Error: could not open file" << std::endl;
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

std::string httpHeaderResponse(std::string code, std::string contentType, std::string content)
{
	//make the header response
	return ("HTTP/1.1 " + code + "\r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content);
}

bool parse_buffer(std::string buffer)
{
	std::istringstream stream(buffer);  // Créer un flux à partir de la chaîne buffer
	std::string line;
	if (!stream)
	{
		std::cout << "Erreur : le flux n'a pas pu être créé." << std::endl;
		return false;
	}

	// Lire chaque ligne du flux
	std::string method;
	std::string path;
	std::string version;

	// Lire chaque ligne du flux
	while (std::getline(stream, line))
	{
		// Chercher "GET" et "HTTP" dans la ligne
		size_t pos1 = line.find("GET");
		size_t pos2 = line.find("HTTP");

		// Si "GET" et "HTTP" sont trouvés, on les analyse
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			// Extraire la méthode (ici on suppose que c'est toujours "GET")
			method = line.substr(pos1, 4);  // La méthode GET a toujours 3 caractères

			// Extraire la ressource demandée entre "GET" et "HTTP"
			path = line.substr(pos1 + 5, pos2 - pos1 - 5);  // Exclure "GET " et " HTTP"
			//GET /favicon.ico HTTP/1.1
			//GET / HTTP/1.1
			
			// Extraire la version HTTP (tout ce qui reste après "HTTP")
			version = line.substr(pos2);  // La version commence juste après "HTTP"

			std::cout << "Méthode: " << method << std::endl;
			std::cout << "Ressource demandée: |" << path << "|" << std::endl;
			std::cout << "Version: " << version << std::endl;

			if (path[0] == ' ')
			{
				std::cout << "Erreur : le chemin est vide entre GET et HTTP." << std::endl;
				std::cout << "Page 404 ouverte" << std::endl;
				return true;
			}
			return false;  // Retourner false si la requête est bien formée
		}
	}
	// Si "GET" et "HTTP" n'ont pas été trouvés
	std::cout << "Requête malformée ou \"GET\" / \"HTTP\" non trouvés" << std::endl;
	return false;
}


int main(int argc, char **argv)
{
	Config conf;
	Server serv;

	if (argc != 2)
	{
		std::cout << "error : use ./webserv file.conf" << std::endl;
		return (0);
	}
	conf.parse_config_file(serv, argv[1]);

	std::cout << "-----------------------------------" << std::endl;

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
	
	std::cout << "Server socket created" << std::endl;
	std::cout << "Server port: " << ntohs(server_address.sin_port) << std::endl;
	
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Erreur lors de la liaison du socket." << std::endl;
		close(server_socket);
		return -1;
	}

	std::cout << "Server socket binded" << std::endl;
	
	if (listen(server_socket, 5) == -1)
	{
		std::cerr << "Erreur lors de la mise en écoute." << std::endl;
		close(server_socket);
		return -1;
	}
	
	std::cout << "Server listening" << std::endl;
	
	while (true)
	{
		sockaddr_in client_address;
		socklen_t client_size = sizeof(client_address);
		int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
		if (client_socket == -1)
		{
			std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
			close(server_socket);
			continue;
		}
		std::cout << "Connexion acceptée !" << std::endl;

		char buffer[1024];
		recv(client_socket, buffer, sizeof(buffer), 0);
		if (!parse_buffer(buffer))
		{
			std::cout << "------------------open ficher 404 error" << std::endl;
			std::string path = "./config/error_page/404.html";
			std::string file_content = readFile(path);
			std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
			std::cout << reponse << std::endl;
			send(client_socket, reponse.c_str(), reponse.size(), 0);
		}
		std::cout << "Requête reçue du client : " << buffer << std::endl;


		std::string path = "." + serv.getRoot() + serv.getIndex();
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << reponse << std::endl;
		send(client_socket, reponse.c_str(), reponse.size(), 0);
		
	}
	return (0);
}