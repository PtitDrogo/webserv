/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:17:01 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/07 13:56:23 by ilbendib         ###   ########.fr       */
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


bool Config::parse_config_file()
{
	std::ifstream file("./config/server.conf");
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
			std::istringstream iss(line);
			std::string word;
			iss >> word;
			int port;
			while (iss >> port)
				this->_ports.push_back(port);
		}
		size_t pos1 = line.find_first_not_of(" \t");
		if (pos1 != std::string::npos && line.find("server_name", pos1) == pos1)
		{
			std::istringstream iss(line.substr(pos1));
			std::string word2;
			iss >> word2;
			std::string adress;
			while (iss >> adress)
				this->_server_names.push_back(adress);
		}
		size_t pos2 = line.find_first_not_of(" \t");
		if (pos2 != std::string::npos && line.find("return", pos2) == pos2)
		{
			std::istringstream iss(line);
			std::string word;
			iss >> word;
			std::string code;
			iss >> code;
			std::string url;
			iss >> url;
			this->_url_redirection[code] = url;
		}
	}
	file.close();
	printVector(this->_ports);
	printVector(this->_server_names);
	printMap(this->_url_redirection);
	return true;
}

std::vector<int> Config::get_ports() const
{
	return this->_ports;
}

std::vector<std::string> Config::get_server_names() const
{
	return this->_server_names;
}


