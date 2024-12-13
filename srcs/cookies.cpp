#include "Cookies.hpp"

Cookies::Cookies()
{
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
		_cookies = other._cookies;
	}
	return (*this);
}

std::map<std::string, Cookie>& Cookies::getCookies()
{
	return (_cookies);
}

// //THIS THROWS AN EXCEPTION USE WITH TRY CATCH
// Cookie& Cookies::getCookie(std::string token)
// {
// 	if (_cookies.find(token) != _cookies.end()) 
//     	return _cookies[token];
// 	else 
// 	{
// 		throw std::out_of_range("Token not found in cookies");
// 	}
// }

void Cookies::addCookie(std::string name, std::string password, std::string token_name)
{
	Cookie new_cookie;

	new_cookie.username = name;
	new_cookie.password = password;
	new_cookie.token	= token_name;
	_cookies[token_name] = new_cookie;
}

void Cookies::deleteCookie(const std::string &token_name)
{
	_cookies.erase(token_name);
}




