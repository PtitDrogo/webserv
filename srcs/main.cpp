/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/11 18:32:36 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/Setup_socket.hpp"
#include "../include/Open_html_page.hpp"

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
	while (true)
	{
		int client_socket = SetupClientAddress(server_socket);
		char buffer[1024];
		int recv_value = recv(client_socket, buffer, sizeof(buffer), 0);
		handleRecvValue(recv_value, client_socket);
		if (parse_buffer(buffer, loc) == 0)
			generate_html_page404(serv, client_socket);
		if (parse_buffer(buffer, loc) == 2)
			generate_html_page_without_location(serv, client_socket);
		else if (parse_buffer(buffer, loc) == 1)
			generate_html_page_with_location(serv, loc, client_socket);
	}
	return (0);
}

