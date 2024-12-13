#include "httpRequest.hpp"

HttpRequest::HttpRequest()
{
	this->_method = "";
	this->_path = "";
	this->_version = "";
	this->_body = "";
	this->_cookies = "";
	this->isCooked = false;
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &copy)
{
	*this = copy;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &copy)
{
	if (this != &copy)
	{
		this->_method = copy._method;
		this->_path = copy._path;
		this->_version = copy._version;
		this->_body = copy._body;
		this->_cookies = copy._cookies;
		this->isCooked = copy.isCooked;
	}
	return *this;
}

std::ostream    &operator<<(std::ostream &o, HttpRequest const &req)
{
	o << "-------REQUEST------" << std::endl;
	o << "Methode = " << req.getMethod() << std::endl;
	o << "Path    = " << req.getPath() << std::endl;
	o << "Version = " << req.getVersion() << std::endl;
	o << "Body    = " << req.getBody() << std::endl;
	o << "Cookies = " << req.getCookies() << std::endl;
	return (o);

}


std::string HttpRequest::getMethod() const
{
	return this->_method;
}

std::string HttpRequest::getPath() const
{
	return this->_path;
}

std::string HttpRequest::getVersion() const
{
	return this->_version;
}

std::string HttpRequest::getBody() const
{
	return this->_body;
}

std::string HttpRequest::getCookies() const
{
	return this->_cookies;
}

bool HttpRequest::getIsCooked() const
{
	return this->isCooked;
}

void HttpRequest::setMethod(std::string method)
{
	this->_method = method;
}

void HttpRequest::setPath(std::string path)
{
	this->_path = path;
}

void HttpRequest::setVersion(std::string version)
{
	this->_version = version;
}

void HttpRequest::setBody(std::string body)
{
	this->_body = body;
}

void HttpRequest::setCookies(std::string cookies)
{
	this->_cookies = cookies;
}

void HttpRequest::setIsCooked(bool isCooked)
{
	this->isCooked = isCooked;
}




