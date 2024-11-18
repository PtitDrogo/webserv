#include "Webserv.hpp"


void printVectorloc2(std::vector<location> loc)
{
	for (size_t i = 0; i < loc.size(); i++)
	{
		std::cout << "Path: " << loc[i].getPath() << std::endl;
		std::cout << "Index: " << loc[i].getIndex() << std::endl;
	}
}

// void printVectorServer2(std::vector<Server> serv)
// {
// 	for (size_t i = 0; i < serv.size(); i++)
// 	{
// 		std::cout << "server { " << std::endl;
// 		std::cout << "Port: " << serv[i].getPort() << std::endl;
// 		std::cout << "Server Name: " << serv[i].getServerName() << std::endl;
// 		std::cout << "Root: " << serv[i].getRoot() << std::endl;
// 		std::cout << "Index: " << serv[i].getIndex() << std::endl;
// 		std::cout << "Error Page: " << serv[i].getErrorPage("403") << std::endl;
// 		printVectorloc2(serv[i].getLocation());
// 		std::cout << "}" << std::endl;
// 	}
// }