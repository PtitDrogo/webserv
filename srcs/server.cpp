#include "server.hpp"

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
		this->_return = copy._return;
		this->_error_page = copy._error_page;
		this->_location = copy._location;
		this->_max_body_size = copy._max_body_size;
		this->_auto_index = copy._auto_index;
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

std::vector<location> &Server::getLocation()
{
	return (this->_location);
}

std::map<std::string ,std::string> Server::getErrorPage() const
{
	return (this->_error_page);
}

int Server::getMaxBodySize() const
{
	return (this->_max_body_size);
}

std::string Server::getAutoIndex() const
{
	return (this->_auto_index);
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

void Server::setLocation(location &loc)
{
	this->_location.push_back(loc);
}

void Server::setMaxBodySize(int max_body_size)
{
	this->_max_body_size = max_body_size;
}

void Server::setAutoIndex(std::string auto_index)
{
	this->_auto_index = auto_index;
}

// void Server::setLocation(std::string loc)
// {
// 	this->_location = loc;
// }

