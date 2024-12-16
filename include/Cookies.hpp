#ifndef COOKIES_HPP
#define COOKIES_HPP

#include "Webserv.hpp"


struct Cookie
{
	std::string username;
	std::string password;
	std::string token;
};


class Cookies
{
	private:
		std::map<std::string, Cookie> _cookies; //token on the left, its cookie on the right.

	public:
		Cookies();
		~Cookies();
		Cookies(const Cookies &other);
		Cookies &operator=(const Cookies &other);

		std::map<std::string, Cookie>& getCookies();
		// Cookie& getCookie(std::string token);

		void addCookie(std::string name, std::string password, std::string token_name);
		void deleteCookie(const std::string &token_name);
};

#endif

