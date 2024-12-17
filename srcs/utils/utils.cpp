#include "Webserv.hpp"

bool isCommentLine(const std::string line)
{
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (line[i] == '#')
			return true;
		else if (std::isspace(line[i]) == false)
			return false;
	}
	return false;
}

bool count_bracket(std::ifstream &file)
{
	std::string line;
	int count = 0;
	int count_server = 0;

	while (std::getline(file, line))
	{
		if (line.find("{") != std::string::npos)
			count++;
		if (line.find("}") != std::string::npos)
			count--;
		if (line.find("server") != std::string::npos)
			count_server++;
	}
	if (count != 0)
	{
		std::cerr << "Error: Invalid bracket" << std::endl;
		return false;
	}
	if (count_server == 0)
	{
		std::cerr << "Error: has nos server" << std::endl;
		return false;
	}
	return true;
}

std::string trim(const std::string& str) 
{
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

bool isExtension(std::string path)
{
	if(path.find(".html") == std::string::npos &&
		path.find(".css") == std::string::npos &&
		path.find(".js") == std::string::npos &&
		path.find(".png") == std::string::npos &&
		path.find(".jpg") == std::string::npos &&
		path.find(".gif") == std::string::npos &&
		path.find(".ico") == std::string::npos &&
		path.find(".py") == std::string::npos)
		return (false);
	return (true);
}