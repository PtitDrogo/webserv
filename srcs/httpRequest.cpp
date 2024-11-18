#include "httpRequest.hpp"

HttpRequest::HttpRequest()
{
	this->_method = "";
	this->_path = "";
	this->_version = "";
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

void HttpRequest::setMethod(std::string metode)
{
	this->_method = metode;
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




