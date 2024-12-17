#include "Cookies.hpp"
#include "Webserv.hpp"

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


std::string httpHeaderResponseForCookies(std::string code, std::string contentType, std::string content, Cookie *cookie)
{
	std::string response = "HTTP/1.1 " + code + "\r\n";
	response += "Content-Type: " + contentType + "\r\n";

	std::stringstream ss;
	ss << content.size();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Connection: close\r\n";
	if (cookie == NULL)
		response += "Set-Cookie: session_token=; sessionID=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/; HttpOnly; Secure\r\n";
	else
		response += "Set-Cookie: session_token=" + cookie->token + "; Path=/; HttpOnly; Secure\r\n";
	response += "\r\n" + content;
	return response;
}

std::string handle_connexion(std::string username, std::string password, Cookies &cook, std::string request_token, Client& client)
{
    std::map<std::string, Cookie>& cookies = cook.getCookies();
	std::map<std::string, Cookie>::iterator it = cookies.find(request_token);
    if (it != cookies.end())
	{
		std::string path = "." + client.getServer().getRoot() + "page/dejaConnecter.html";
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, &it->second);
		return response;
	}
	if (password.empty() == false && username.empty() == false)
	{
		std::string response;
		std::string path = "." + client.getServer().getRoot() + "page/IsConnected.html";
		std::string session_token = "token_" + username;
		cook.addCookie(username, password, session_token);
		std::string file_content = readFile(path);
		response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, &(cookies[session_token]));
		return response;
	}
	return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<h1>Veuillez fournir un nom d'utilisateur et un mot de passe</h1>";
}


std::string handle_deconnexion(Cookies &cook, std::string request_token, Client& client)
{
    std::map<std::string, Cookie> cookies = cook.getCookies();
	std::map<std::string, Cookie>::iterator it = cookies.find(request_token);
    
    if (it != cookies.end())
    {
        cook.deleteCookie(request_token);
        std::string path = "." + client.getServer().getRoot() + "page/isDeconnected.html";
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, NULL);
		return response;
	}
	else
	{
		std::string path = "." + client.getServer().getRoot() + "page/PasConnecter.html";
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		return response;
	}
}

