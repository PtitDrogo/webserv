/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 19:34:37 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"
#include "../include/Setup_socket.hpp"
#include "../include/Open_html_page.hpp"

// std::string getContentType(std::string &path) 
// {
// 	std::map<std::string, std::string> contentTypes;

// 	contentTypes.insert(std::pair<std::string, std::string>(".html", "text/html"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".css", "text/css"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".png", "image/png"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".gif", "image/gif"));

// 	size_t dotPos = path.find_last_of(".");
// 	if (dotPos != std::string::npos) {
// 		std::string extension = path.substr(dotPos);
// 		if (contentTypes.find(extension) != contentTypes.end()) {
// 			return contentTypes[extension];
// 		}
// 	}
// 	//if i dont have exetension i add backslash
// 	else
// 	{
// 		//if size of path = 0 or no '/' at the end
// 		if(path.size() == 0 || path.at(path.size() - 1) != '/')
// 			path += "/";
		
// 	}
// 	return "text/html"; // Default content type
// }

void parse_buffer(std::string buffer, Server &serv , int client_socket)
{
	std::istringstream stream(buffer);
	std::string line;

	if (!stream)
	{
		std::cout << "Erreur : le flux n'a pas pu être créé." << std::endl;
		return ;
	}
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string name;
	std::string email;
	std::string message;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("GET");
		size_t pos2 = line.find("HTTP");
		
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			method = line.substr(pos1, 4);
			path = line.substr(pos1 + 4, pos2 - pos1 - 5);
			version = line.substr(pos2);
			if (path == "/")
			{
				if (!serv.getIndex().empty())
					finalPath = "." + serv.getRoot() + serv.getIndex();
				else
					generate_html_page_error(serv, client_socket, "404");
			}
			else
				finalPath = "." + serv.getRoot() + path;
		}
		size_t pos3 = line.find("name=");
		size_t pos4 = line.find("email=");
		size_t pos5 = line.find("message=");
		if (pos3 != std::string::npos && pos4 != std::string::npos && pos5 != std::string::npos)
		{
			size_t start_pos = pos3 + 5;
			size_t end_pos = line.find('&', start_pos);
			name = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos4 + 6;
			end_pos = line.find('&', start_pos);
			email = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos5 + 8;
			end_pos = line.find('\0', start_pos);
			message = line.substr(start_pos, end_pos - start_pos);
		}
	}

	std::cout << "------------voici le path------------|" << finalPath << "|" << std::endl;
	std::string file_content = readFile(finalPath);
	if (file_content.empty())
		generate_html_page_error(serv, client_socket, "404");
	std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);


	std::ofstream outfile ("./config/message.txt", std::ios::app);
	if (outfile.is_open() && name != "" && email != "" && message != "")
	{
		outfile << "name : " << name << std::endl;
		outfile << "email : " << email << std::endl;
		outfile << "message : " << message << std::endl;
		outfile << "--------------------------------------" << std::endl;
		outfile.close();
		std::string reponse2 = 
			"HTTP/1.1 201 Found\r\n"
			"Location: /index.html\r\n" // Redirige vers index.html
			"Content-Length: 0\r\n"
			"Connection: close\r\n"
			"\r\n";
		send(client_socket, reponse2.c_str(), reponse2.size(), 0);
	}
	else
		std::cout << "Unable to open file" << std::endl;
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
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		parse_buffer(buffer, serv, client_socket);
	}
	return (0);
}
