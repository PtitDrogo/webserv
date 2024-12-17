#include "Webserv.hpp"

void printVector(std::map<std::string, std::string> errorPage)
{
	for (std::map<std::string, std::string>::iterator it = errorPage.begin(); it != errorPage.end(); ++it)
	{
		std::cout << "	Error Page: " << it->first << " => " << it->second << std::endl;
	}
}

void printMapRedirect(std::map<std::string, std::string> redirect)
{
	for (std::map<std::string, std::string>::iterator it = redirect.begin(); it != redirect.end(); ++it)
	{
		std::cout << "	Redirect: " << it->first << " => " << it->second << std::endl;
	}
}

void printVectorloc(std::vector<location> loc)
{
	for (size_t i = 0; i < loc.size(); i++)
	{
		std::cout << "location { " << std::endl;
		std::cout << "	Path: " << loc[i].getPath() << std::endl;
		std::cout << "	Index: " << loc[i].getIndex() << std::endl;
		std::cout << "	Root: " << loc[i].getRoot() << std::endl;
		std::cout << "	Auto Index: " << loc[i].getAutoIndex() << std::endl;
		std::cout << "	Allow Method: " << loc[i].getAllowMethod() << std::endl;
		std::cout << "	Cgi Path: " << loc[i].getCgiPath() << std::endl;
		printMapRedirect(loc[i].getRedir());
		std::cout << "}" << std::endl;
	}
}

void printVectorServer(std::vector<Server> serv)
{
	for (size_t i = 0; i < serv.size(); i++)
	{
		std::cout << "server { " << std::endl;
		std::cout << "	Port: " << serv[i].getPort() << std::endl;
		std::cout << "	my host: " << serv[i].getHost() << std::endl;
		std::cout << "	Server Name: " << serv[i].getServerName() << std::endl;
		std::cout << "	Root: " << serv[i].getRoot() << std::endl;
		std::cout << "	Index: " << serv[i].getIndex() << std::endl;
		std::cout << "	Max Body Size: " << serv[i].getMaxBodySize() << std::endl;
		std::cout << "	Auto Index: " << serv[i].getAutoIndex() << std::endl;

		printVector(serv[i].getErrorPage());
		printVectorloc(serv[i].getLocation());

		std::cout << "}" << std::endl;
	}
}

void printMapCgi(std::map<std::string, std::string> cgi)
{
	for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); ++it)
	{
		std::cout << "Cgi: " << it->first << " => " << it->second << std::endl;
	}
}