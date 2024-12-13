#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Webserv.hpp"

class HttpRequest
{
	private :
		std::string _method;
		std::string _path;
		std::string _version;
		std::string _body;
		std::string _cookies;
		bool isCooked;
	public :
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest &copy);
		HttpRequest &operator=(const HttpRequest &copy);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getBody() const;
		std::string getCookies() const;
		bool getIsCooked() const;

		void setMethod(std::string method);
		void setPath(std::string path);
		void setVersion(std::string version);
		void setBody(std::string body);
		void setCookies(std::string cookies);
		void setIsCooked(bool isCooked);
};

std::ostream    &operator<<(std::ostream &o, HttpRequest const &req);
#endif