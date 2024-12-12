#include "Cookies.hpp"

Cookies::Cookies()
{
	this->_cookies.clear();
	this->isConnect = false;
}

Cookies::~Cookies()
{
}

Cookies::Cookies(const Cookies &other)
{
	*this = other;
}

Cookies &Cookies::operator=(const Cookies &other)
{
	if (this != &other)
	{
		this->_cookies = other._cookies;
		this->isConnect = other.isConnect;
	}
	return (*this);
}

std::map<std::string, std::string> Cookies::getCookies() const
{
	return (this->_cookies);
}

bool Cookies::getIsConnect() const
{
	return (this->isConnect);
}

void Cookies::setCookies(std::string name, std::string value)
{
	this->_cookies[name] = value;
}

void Cookies::setIsConnect(bool isConnect)
{
	this->isConnect = isConnect;
}

void Cookies::deleteCookie(const std::string &name)
{
	std::map<std::string, std::string>::iterator it = _cookies.find(name);

	if (it != _cookies.end())
	{
		_cookies.erase(it);
		std::cout << "Cookie " << name << " supprimé avec succès." << std::endl;
	}
	else
	{
		std::cout << "Cookie " << name << " non trouvé." << std::endl;
	}
}