/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 12:35:58 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/08 15:15:56 by ilbendib         ###   ########.fr       */
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

std::map<std::string, std::string> Server::getErrorPage() const 
{
	return (this->_error_page);
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

