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
	public :
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest &copy);
		HttpRequest &operator=(const HttpRequest &copy);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getBody() const;

		void setMethod(std::string metode);
		void setPath(std::string path);
		void setVersion(std::string version);
		void setBody(std::string body);
};

std::ostream    &operator<<(std::ostream &o, HttpRequest const &req);
#endif