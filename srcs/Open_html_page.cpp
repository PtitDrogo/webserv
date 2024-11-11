/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Open_html_page.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:21:12 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/11 18:21:15 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/server.hpp"

std::string readFile(std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		return ("");
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

std::string httpHeaderResponse(std::string code, std::string contentType, std::string content)
{
	return ("HTTP/1.1 " + code + "\r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content);
}

void generate_html_page404(Server &serv, int client_socket)
{
	std::cout << "------------------open ficher 404 error" << std::endl;
	std::string path = "." + serv.getErrorPage("404");
	std::string file_content = readFile(path);
	std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void generate_html_page_without_location(Server &serv, int client_socket)
{
	std::string path = "." + serv.getRoot() + serv.getIndex();
	std::string file_content = readFile(path);
	//////////////////////////////////////////////////////////////////
	///////////CHECK SI J'AI LES PERM POUR ACCEDER AU DOSSIER HTML////
	//////////////////////////////////////////////////////////////////
	if (file_content.empty())
	{
		std::string path = "." + serv.getErrorPage("403");
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client_socket, reponse.c_str(), reponse.size(), 0);
	}
	std::string reponse2 = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse2.c_str(), reponse2.size(), 0);
}

void generate_html_page_with_location(Server &serv, location &loc, int client_socket)
{
	std::string path = "." + serv.getRoot() + loc.getIndex();
	std::string file_content = readFile(path);
	//////////////////////////////////////////////////////////////////
	///////////CHECK SI J'AI LES PERM POUR ACCEDER AU DOSSIER HTML////
	//////////////////////////////////////////////////////////////////
	if (file_content.empty())
	{
		std::string path = "." + serv.getErrorPage("403");
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client_socket, reponse.c_str(), reponse.size(), 0);
	}
	std::string reponse2 = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse2.c_str(), reponse2.size(), 0);
}