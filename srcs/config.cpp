/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:17:01 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/13 13:58:42 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

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


bool Config::parse_config_file(Server &serv, location &loc, std::string filename)
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
			size_t start = pos + 6;
			while (start < line.size() && std::isspace(line[start]))
				start++;
			size_t end = line.find_first_of(" \t", start);
			std::string port = line.substr(start, end - start);
			serv.setPort(port);
		}
		size_t pos1 = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line.find("server_name", pos1) == pos1)
		{
			size_t start = pos1 + 11;
			while (start < line.size() && std::isspace(line[start]))
				start++;
			size_t end = line.find_first_of(" \t", start);
			std::string server_name = line.substr(start, end - start);
			serv.setServerName(server_name);
		}
		size_t pos2 = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line.find("index", pos2) == pos2)
		{
			size_t start = pos2 + 5;
			while (start < line.size() && std::isspace(line[start]))
				start++;
			size_t end = line.find_first_of(" \t", start);
			std::string index = line.substr(start, end - start);
			serv.setIndex(index);
		}
		size_t pos3 = line.find_first_not_of(" \t");
		if (pos3 != std::string::npos && line.find("root", pos3) == pos3)
		{
			size_t start = pos3 + 4;
			while (start < line.size() && std::isspace(line[start]))
				start++;
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
		}
		///////////////////////////////
		////////RECUPE LOCATION////////
		///////////////////////////////
		size_t pos5 = line.find_first_not_of(" \t");
		if (pos5 != std::string::npos && line.find("location", pos5) == pos5)
		{
			size_t start = pos5 + 8;
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}
			size_t end = line.find_first_of(" {", start);
			std::string path = line.substr(start, end - start) + ' ';
			loc.setPath(path);

			// Si l'accolade ouvrante est sur la même ligne, chercher le reste
			size_t braceOpenPos = line.find('{', end);
			std::string sectionContent;

			if (braceOpenPos != std::string::npos) {
				sectionContent += line.substr(braceOpenPos + 1) + "\n";
			}
			std::istream& input = file;
			// Lire jusqu'à trouver l'accolade fermante
			std::string nextLine;
			while (std::getline(input, nextLine)) {
				size_t braceClosePos = nextLine.find('}');
				if (braceClosePos != std::string::npos) {
					sectionContent += nextLine.substr(0, braceClosePos);
					break;
				} else {
					sectionContent += nextLine + "\n";
				}
			}

			// Parse la section pour l'index
			std::istringstream sectionStream(sectionContent);
			std::string subLine;
			while (std::getline(sectionStream, subLine)) {
				size_t indexPos = subLine.find("index");
				if (indexPos != std::string::npos) {
					size_t startIndex = subLine.find_first_not_of(" \t", indexPos + 5);
					size_t endIndex = subLine.find_first_of(" \t;", startIndex);
					std::string index = subLine.substr(startIndex, endIndex - startIndex);
					loc.setIndex(index);
					break; // Arrête la recherche après avoir trouvé la première occurrence de l'index
				}
			}
		}
		std::cout << line << std::endl;
	}
	std::cout << "\n\n\n\n" << std::endl;
	return (true);
}
