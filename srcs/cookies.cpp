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
	std::map<std::string, std::string>::iterator it = _cookies.find(name);  // Spécifie le type de l'itérateur

	if (it != _cookies.end()) {  // Vérifie si l'itérateur n'est pas à la fin
		_cookies.erase(it);      // Supprime l'élément de la map
		std::cout << "Cookie " << name << " supprimé avec succès." << std::endl;
	} else {
		std::cout << "Cookie " << name << " non trouvé." << std::endl;
	}
}