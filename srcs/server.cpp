/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 12:35:58 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 13:06:49 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

Server::Server() 
{
	// this->_index = "";
	// this->_port = "";
	// this->_root = "";
	// this->_server_name = "";
}

Server::~Server() {}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server &Server::operator=(const Server &copy)
{
	if (this != &copy)
	{
		this->_index = copy._index;
		this->_port = copy._port;
		this->_root = copy._root;
		this->_server_name = copy._server_name;
	}
	return *this;
}

std::string Server::getPort() const 
{
	return (this->_port);
}

std::string Server::getIndex() const 
{
	return (this->_index);
}

std::string Server::getRoot() const 
{
	return (this->_root);
}

std::string Server::getServerName() const 
{
	return (this->_server_name);
}

std::map<std::string , std::string> Server::getReturn() const 
{
	return (this->_return);
}

// std::string Server::getLocation() const 
// {
// 	return (this->_location);
// }

std::string Server::getErrorPage(const std::string& errorCode) const {
    std::map<std::string, std::string>::const_iterator it = _error_page.find(errorCode);
    
    // Vérifie si l'élément avec la clé "errorCode" existe dans la map
    if (it != _error_page.end()) {
        return it->second;  // Retourne le deuxième élément (la valeur)
    }
    
    // Si le code d'erreur n'est pas trouvé, retourne une valeur par défaut
    return "error_default.html";
}
void Server::setPort(std::string port)
{
	this->_port = port;
}

void Server::setIndex(std::string index)
{
	this->_index = index;
}

void Server::setRoot(std::string root)
{
	this->_root = root;
}

void Server::setServerName(std::string server_name)
{
	this->_server_name = server_name;
}

void Server::setReturn(std::map<std::string , std::string> return_)
{
	this->_return = return_;
}

void Server::setErrorPage(std::string& error_code, std::string& error_file)
{
	_error_page[error_code] = error_file;
}

void Server::setLocation(location loc)
{
	this->_location.push_back(loc);
}

// void Server::setLocation(std::string loc)
// {
// 	this->_location = loc;
// }

