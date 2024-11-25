#include "location.hpp"

location::location() {}

location::~location() {}

location::location(const location &copy)
{
	*this = copy;
}

location &location::operator=(const location &copy)
{
	if (this != &copy)
	{
		this->_index = copy._index;
		this->path = copy.path;
		this->_root = copy._root;
		this->_auto_index = copy._auto_index;
		this->_allow_method = copy._allow_method;
		this->_cgi_path = copy._cgi_path;
	}
	return *this;
}

std::string location::getIndex() const
{
	return (this->_index);
}

std::string location::getPath() const
{
	return (this->path);
}

std::string location::getRoot() const
{
	return (this->_root);
}

std::string location::getAutoIndex() const
{
	return (this->_auto_index);
}

std::string location::getAllowMethod() const
{
	return (this->_allow_method);
}

std::string location::getCgiPath() const
{
	return (this->_cgi_path);
}

void location::setRoot(std::string root)
{
	this->_root = root;
}

void location::setIndex(std::string index)
{
	this->_index = index;
}

void location::setPath(std::string path)
{
	this->path = path;
}

void location::setAutoIndex(std::string auto_index)
{
	this->_auto_index = auto_index;
}

void location::setAllowMethod(std::string allow_method)
{
	this->_allow_method = allow_method;
}

void location::setCgiPath(std::string cgi_path)
{
	this->_cgi_path = cgi_path;
}

