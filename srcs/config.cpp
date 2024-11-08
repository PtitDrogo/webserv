/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:17:01 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/08 15:17:07 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"

Config::Config() {}

Config::~Config() {}

Config::Config(const Config &copy)
{
	*this = copy;
}

Config &Config::operator=(const Config &copy)
{
	if (this != &copy)
	{
		*this = copy;
	}
	return *this;
}

std::vector<Server> Config::getServer() const
{
	return this->_server;
}


bool Config::parse_config_file(Server &serv, std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open file" << std::endl;
		return false;
	}
	if (file.is_open())
	{
		std::cout << "File opened successfully" << std::endl;
	}
	std::string line;
	while (std::getline(file, line))
	{
		size_t pos = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line.find("listen", pos) == pos)
		{
			size_t start = pos + 6; // Position après "listen"
			
			// Ignorer les espaces après "listen"
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}

			// Récupérer le port jusqu'au prochain espace ou la fin de la ligne
			size_t end = line.find_first_of(" \t", start);
			std::string port = line.substr(start, end - start);
			serv.setPort(port);
		}
		size_t pos1 = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line.find("server_name", pos1) == pos1)
		{
			size_t start = pos1 + 11; // Position après "listen"
			
			// Ignorer les espaces après "listen"
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}

			// Récupérer le port jusqu'au prochain espace ou la fin de la ligne
			size_t end = line.find_first_of(" \t", start);
			std::string server_name = line.substr(start, end - start);
			serv.setServerName(server_name);
		}
		size_t pos2 = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line.find("index", pos2) == pos2)
		{
			size_t start = pos2 + 5; // Position après "listen"
			
			// Ignorer les espaces après "listen"
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}

			// Récupérer le port jusqu'au prochain espace ou la fin de la ligne
			size_t end = line.find_first_of(" \t", start);
			std::string index = line.substr(start, end - start);
			serv.setIndex(index);
		}
		size_t pos3 = line.find_first_not_of(" \t");
		if (pos3 != std::string::npos && line.find("root", pos3) == pos3)
		{
			size_t start = pos3 + 4; // Position après "listen"
			
			// Ignorer les espaces après "listen"
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}

			// Récupérer le port jusqu'au prochain espace ou la fin de la ligne
			size_t end = line.find_first_of(" \t", start);
			std::string root = line.substr(start, end - start);
			serv.setRoot(root);
		}
		size_t pos4 = line.find_first_not_of(" \t");
		if (pos4 != std::string::npos && line.find("error_page", pos4) == pos4)
		{
			std::istringstream iss(line);
			std::string word;
			iss >> word;
			std::string error_code;
			iss >> error_code;
			std::string error_file;
			iss >> error_file;
			serv.setErrorPage(error_code, error_file);
			std::cout << "Error page for code " << error_code << " is " << error_file << std::endl;
		}

		
	}
	std::cout << "port = " << serv.getPort() << std::endl;
	std::cout << "server_name = "<< serv.getServerName() << std::endl;
	std::cout << "index = " << serv.getIndex() << std::endl;
	std::cout << "root = " << serv.getRoot() << std::endl;
	printMap(serv.getErrorPage());
	return (true);
}

// bool Config::parse_config_file()
// {
// 	std::ifstream file("./config/server.conf");
// 	if (!file.is_open())
// 	{
// 		std::cerr << "Error: could not open file" << std::endl;
// 		return false;
// 	}
// 	if (file.is_open())
// 	{
// 		std::cout << "File opened successfully" << std::endl;
// 	}
// 	std::string line;
// 	while (std::getline(file, line))
// 	{
// 		size_t pos = line.find_first_not_of(" \t");
// 		if (pos != std::string::npos && line.find("listen", pos) == pos)
// 		{
// 			std::istringstream iss(line);
// 			std::string word;
// 			iss >> word;
// 			int port;
// 			while (iss >> port)
// 				this->_ports.push_back(port);
// 		}
// 		size_t pos1 = line.find_first_not_of(" \t");
// 		if (pos1 != std::string::npos && line.find("server_name", pos1) == pos1)
// 		{
// 			std::istringstream iss(line.substr(pos1));
// 			std::string word2;
// 			iss >> word2;
// 			std::string adress;
// 			while (iss >> adress)
// 				this->_server_names.push_back(adress);
// 		}
// 		size_t pos2 = line.find_first_not_of(" \t");
// 		if (pos2 != std::string::npos && line.find("return", pos2) == pos2)
// 		{
// 			std::istringstream iss(line);
// 			std::string word;
// 			iss >> word;
// 			std::string code;
// 			iss >> code;
// 			std::string url;
// 			iss >> url;
// 			this->_url_redirection[code] = url;
// 		}
// 	}
// 	file.close();
// 	printVector(this->_ports);
// 	printVector(this->_server_names);
// 	printMap(this->_url_redirection);
// 	return true;
// }

// std::vector<int> Config::get_ports() const
// {
// 	return this->_ports;
// }

// std::vector<std::string> Config::get_server_names() const
// {
// 	return this->_server_names;
// }


