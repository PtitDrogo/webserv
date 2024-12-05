#ifndef COOKIES_HPP
#define COOKIES_HPP

#include "Webserv.hpp"

class Cookies
{
	private:
		std::map<std::string, std::string> _cookies;
		bool isConnect;

	public:
		Cookies();
		~Cookies();
		Cookies(const Cookies &other);
		Cookies &operator=(const Cookies &other);

		std::map<std::string, std::string> getCookies() const;
		bool getIsConnect() const;

		void setCookies(std::string name, std::string value);
		void setIsConnect(bool isConnect);
		void deleteCookie(const std::string &name);
};

#endif

