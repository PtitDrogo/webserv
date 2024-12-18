#include "location.hpp"

location::location() {}

location::~location() {}

location::location(const location &copy)
{
	*this = copy;
}

location &location::operator=(const location &copy)
{
	if (this != &copy)
	{
		this->_index = copy._index;
		this->path = copy.path;
		this->_root = copy._root;
		this->_auto_index = copy._auto_index;
		this->_allow_method = copy._allow_method;
		this->_cgi_path = copy._cgi_path;
		this->_redir = copy._redir;
	}
	return *this;
}

std::string location::getIndex() const
{
	return (this->_index);
}

std::string location::getPath() const
{
	return (this->path);
}

std::string location::getRoot() const
{
	return (this->_root);
}

std::string location::getAutoIndex() const
{
	return (this->_auto_index);
}

std::string location::getAllowMethod() const
{
	return (this->_allow_method);
}

std::string location::getCgiPath() const
{
	return (this->_cgi_path);
}

std::map<std::string, std::string> location::getRedir() const
{
	return (this->_redir);
}


void location::setRoot(std::string root)
{
	this->_root = root;
}

void location::setIndex(std::string index)
{
	this->_index = index;
}

void location::setPath(std::string path)
{
	this->path = path;
}

void location::setAutoIndex(std::string auto_index)
{
	this->_auto_index = auto_index;
}

void location::setAllowMethod(std::string allow_method)
{
	this->_allow_method = allow_method;
}

void location::setCgiPath(std::string cgi_path)
{
	this->_cgi_path = cgi_path;
}

void location::setRedir(std::string& error_code, std::string& path)
{
	this->_redir[error_code] = path;
}

std::string CheckLocation(const std::string& path, std::vector<location>& locationPath, Client& client)
{
	std::string cleanedPath = trim(path);
	for (size_t i = 0; i < locationPath.size(); ++i)
	{
		std::string locationStr = locationPath[i].getPath();
		locationStr = trim(locationStr);
		if (cleanedPath == locationStr)
		{
			if (cleanedPath.size() <= locationStr.size())
			{
				client.setLocation(&locationPath[i]);
				return "." + locationPath[i].getRoot();
			}
			std::string relativePath = cleanedPath.substr(locationStr.size());
			if (relativePath.empty() || relativePath == "/")
			{
				if (!locationPath[i].getIndex().empty())
				{
					client.setLocation(&locationPath[i]);
					return "." + locationPath[i].getRoot() + locationPath[i].getIndex();
				}
				client.setLocation(&locationPath[i]);
				return "." + locationPath[i].getRoot();
			}
			client.setLocation(&locationPath[i]);
			return "." + locationPath[i].getRoot() + relativePath;
		}
	}
	return "";
}

bool sendRedirection(int client_socket, const std::string& path)
{
	std::ostringstream responseStream;
	responseStream << "HTTP/1.1 301 Moved Permanently\r\n"
				<< "Location: " << path << "\r\n"
				<< "Content-Type: text/html\r\n"
				<< "Content-Length: 0\r\n"
				<< "Connection: close\r\n"
				<< "\r\n";
	std::string response = responseStream.str();
	if (send(client_socket, response.c_str(), response.size(), 0) == -1)
		return false;
	return true;
}

std::string parse_no_location(std::string path, Client &client, std::string finalPath, int client_socket)
{
	std::string reponse;
	std::string file_content;
	Server& server = client.getServer();

	if (path == "/")
	{
		if (!server.getIndex().empty())
		{
			finalPath = "." + server.getRoot() + server.getIndex();
			return finalPath;
		}
		if (server.getAutoIndex() == "on")
		{
			autoIndex(path, client);
			return "";
		}
		else
		{
			generate_html_page_error(client, "404");
		}
	}
	else if (server.getAutoIndex() == "on" && server.getIndex().empty())
	{
		finalPath = "." + server.getRoot() + path;
		if (!isExtension(finalPath))
		{
			std::vector<std::string> files = listDirectory(finalPath);
			file_content = generateAutoIndexPage(finalPath, files, client);
		}
		else if (isExtension(finalPath))
			file_content = readFile(finalPath);
		reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		if (send(client_socket, reponse.c_str(), reponse.size(), 0) == -1)
		{
			std::cerr << "Error sending back the response with Auto index" << std::endl;
			return "";
		}
		return "";
	}
	else
		finalPath = "." + server.getRoot() + path;
	return finalPath;
}


bool isMethodAllowed(const std::string& allowedMethods, const std::string& reqMethod)
{
    std::string trimmedReqMethod = reqMethod;
    trimmedReqMethod.erase(0, trimmedReqMethod.find_first_not_of(" \t"));
    trimmedReqMethod.erase(trimmedReqMethod.find_last_not_of(" \t") + 1);
    
    std::stringstream ss(allowedMethods);
    std::string method;

	while (std::getline(ss, method, ' '))
	{
		method.erase(0, method.find_first_not_of(" \t"));
		method.erase(method.find_last_not_of(" \t") + 1);
		if (method == trimmedReqMethod)
			return true;
	}
	return false;
}

std::string parse_with_location(Client &client, std::string finalPath, HttpRequest &req)
{
	location location = *(client.getLocation());

	if (isMethodAllowed(location.getAllowMethod(), req.getMethod()) == false && location.getAllowMethod().empty() == false)
	{
		generate_html_page_error(client, "404");
		return "";
	}
	if (location.getRedir().empty() == false)
	{
		std::map<std::string, std::string> redirMap = location.getRedir();
		for (std::map<std::string, std::string>::iterator it = redirMap.begin(); it != redirMap.end(); ++it)
		{
			std::string errorCode = it->first;
			std::string path = it->second;
			if (errorCode == "301" && path != "")
				sendRedirection(client.getSocket(), path);
			return "";
		}
	}
	if (location.getIndex().empty() == false)
	{
		finalPath = "." + location.getRoot() + location.getIndex();
		return finalPath;
	}
	else if (location.getIndex().empty() == true)
	{
		if (location.getAutoIndex() == "on")
		{
			autoIndex(finalPath, client);
			return "";
		}
		else
		{
			generate_html_page_error(client, "404");
			return "";
		}
	}
	return finalPath;
}