#include "Webserv.hpp"
#include "include.hpp"


void parse_buffer_get(std::string buffer, Server &serv , int client_socket)
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
	}
	std::cout << "------------voici le path------------|" << finalPath << "|" << std::endl;
	std::string file_content = readFile(finalPath);
	if (file_content.empty())
		generate_html_page_error(serv, client_socket, "404");
	std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void parse_buffer_post(std::string buffer , int client_socket, Server &serv)
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
	std::string filename;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("FileName=");
		size_t pos3 = line.find("name=");
		size_t pos4 = line.find("email=");
		size_t pos5 = line.find("message=");
		if (pos3 != std::string::npos && pos4 != std::string::npos && pos5 != std::string::npos)
		{
			size_t start_pos = pos1 + 9;
			size_t end_pos = line.find('&', start_pos);
			filename = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos3 + 5;
			end_pos = line.find('&', start_pos);
			name = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos4 + 6;
			end_pos = line.find('&', start_pos);
			email = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos5 + 8;
			end_pos = line.find('\0', start_pos);
			message = line.substr(start_pos, end_pos - start_pos);
		}
	}
	
	if (!filename.empty())
	{
		filename = "./config/base_donnees/" + filename + ".txt";
		std::ofstream outfile (filename.c_str(), std::ios::app);
		if (outfile.is_open())
		{
			outfile << "name : " << name << std::endl;
			outfile << "email : " << email << std::endl;
			outfile << "content : " << message << std::endl;
			outfile << "--------------------------------------" << std::endl;
			outfile.close();

			std::string path = "." + serv.getRoot() + serv.getIndex();
			std::string file_content = readFile(path);
			std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
			send(client_socket, reponse.c_str(), reponse.size(), 0);
		}
	}
	else
		std::cout << "Unable to open file" << std::endl;
	filename.clear();
	name.clear();
	email.clear();
	message.clear();
}