#include "Webserv.hpp"


void printVectorloc2(std::vector<location> loc)
{
	for (size_t i = 0; i < loc.size(); i++)
	{
		std::cout << "Path: " << loc[i].getPath() << std::endl;
		std::cout << "Index: " << loc[i].getIndex() << std::endl;
	}
}