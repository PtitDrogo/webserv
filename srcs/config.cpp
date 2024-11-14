#include "Webserv.hpp"
#include "config.hpp"

Config::Config() {}

Config::~Config() {}

Config::Config(const Config &copy)
{
	*this = copy;
}

Config &Config::operator=(const Config &copy)
{
	if (this != &copy)
	{
		this->_server = copy._server;
	}
	return *this;
}

void printVector(std::map<std::string, std::string> errorPage)
{
	for (std::map<std::string, std::string>::iterator it = errorPage.begin(); it != errorPage.end(); ++it)
	{
		std::cout << "Error Page: " << it->first << " => " << it->second << std::endl;
	}
}

std::vector<Server> &Config::getServer()
{
	return this->_server;
}

void Config::setServer(Server &serv)
{
	this->_server.push_back(serv);
}

void printVectorloc(std::vector<location> loc)
{
	for (size_t i = 0; i < loc.size(); i++)
	{
		std::cout << "Path: " << loc[i].getPath() << std::endl;
		std::cout << "Index: " << loc[i].getIndex() << std::endl;
	}
}

void printVectorServer(std::vector<Server> serv)
{
    for (size_t i = 0; i < serv.size(); i++)
    {
        std::cout << "server { " << std::endl;
        std::cout << "Port: " << serv[i].getPort() << std::endl;
        std::cout << "Server Name: " << serv[i].getServerName() << std::endl;
        std::cout << "Root: " << serv[i].getRoot() << std::endl;
        std::cout << "Index: " << serv[i].getIndex() << std::endl;
        
		printVector(serv[i].getErrorPage());


        printVectorloc(serv[i].getLocation());

        std::cout << "}" << std::endl;
    }
}

void parse_listen(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("listen", pos) == pos)
	{
		size_t start = pos + 6;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string port = line.substr(start, end - start);
		serv.setPort(port);
	}
	// std::cout << "Port: " << serv.getPort() << std::endl;
}

void parse_server_name(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("server_name", pos) == pos)
	{
		size_t start = pos + 11;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string server_name = line.substr(start, end - start);
		serv.setServerName(server_name);
	}
	// std::cout << "Server Name: " << serv.getServerName() << std::endl;
}

void parse_index(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("index", pos) == pos)
	{
		size_t start = pos + 5;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string index = line.substr(start, end - start);
		serv.setIndex(index);
	}
	// std::cout << "Index: " << serv.getIndex() << std::endl;
}

void parse_root(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("root", pos) == pos)
	{
		size_t start = pos + 4;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string root = line.substr(start, end - start);
		serv.setRoot(root);
	}
	// std::cout << "Root: " << serv.getRoot() << std::endl;
}



void parse_error_page(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("error_page", pos) == pos)
	{
		std::istringstream iss(line);
		std::string word;
		iss >> word;
		std::string error_code;
		iss >> error_code;
		std::string error_file;
		iss >> error_file;
		serv.setErrorPage(error_code, error_file);
		printVector(serv.getErrorPage());
	}
}

void parse_location(std::string line, Server &serv, std::ifstream &file)
{
	size_t pos5 = line.find_first_not_of(" \t");
	if (pos5 != std::string::npos && line.find("location", pos5) == pos5)
	{
		location loc;
		size_t start = pos5 + 8;
		while (start < line.size() && std::isspace(line[start])) {
			start++;
		}
		size_t end = line.find_first_of(" {", start);
		std::string path = line.substr(start, end - start) + ' ';
		loc.setPath(path);
		// std::cout << "Path: " << loc.getPath() << std::endl;
		size_t braceOpenPos = line.find('{', end);
		std::string sectionContent;

		if (braceOpenPos != std::string::npos) {
			sectionContent += line.substr(braceOpenPos + 1) + "\n";
		}
		std::istream& input = file;
		std::string nextLine;
		while (std::getline(input, nextLine)) {
			size_t braceClosePos = nextLine.find('}');
			if (braceClosePos != std::string::npos) {
				sectionContent += nextLine.substr(0, braceClosePos);
				break;
			} else {
				sectionContent += nextLine + "\n";
			}
		}

		std::istringstream sectionStream(sectionContent);
		std::string subLine;
		while (std::getline(sectionStream, subLine)) {
			size_t indexPos = subLine.find("index");
			if (indexPos != std::string::npos) {
				size_t startIndex = subLine.find_first_not_of(" \t", indexPos + 5);
				size_t endIndex = subLine.find_first_of(" \t;", startIndex);
				std::string index = subLine.substr(startIndex, endIndex - startIndex);
				loc.setIndex(index);
				// std::cout << "Index: " << loc.getIndex() << std::endl;
				break;
			}
		}
		// std::cout << "je suis la" << std::endl;	
		serv.setLocation(loc);
	}
}

void Config::createServerr(std::ifstream &file , Server &serv)
{
	std::string line;

	std::cout << "createServerr" << std::endl;
	while(std::getline(file, line))
	{
		if (line.find("listen") != std::string::npos)
			parse_listen(line, serv);
		if (line.find("server_name") != std::string::npos)
			parse_server_name(line, serv);
		if (line.find("index") != std::string::npos)
			parse_index(line, serv);
		if (line.find("root") != std::string::npos)
			parse_root(line, serv);
		if (line.find("error_page") != std::string::npos)
			parse_error_page(line, serv);
		if (line.find("location") != std::string::npos)
			parse_location(line, serv, file);
		if (line.find("}") != std::string::npos)
		{
			// std::cout << "je suis ici" << std::endl;
			break;
		}
		// std::cout << line << std::endl;
	}
	this->setServer(serv);
	// std::cout << "je suis ici" << std::endl;
}

void Config::printConfig()
{
	std::cout << "printConfig" << std::endl;

	std::vector<Server> serv = this->getServer();
	std::ifstream file("./config/server.conf");

	std::string line;
	while (std::getline(file, line))
	{
		std::cout << line << std::endl;
	}

	std::cout << "--------------------------------" << std::endl;
	printVector(serv[0].getErrorPage());
	std::cout << "--------------------------------" << std::endl;	
	printVectorServer(serv);
	printVectorloc(serv[0].getLocation());
}

// bool Config::parse_config_file(Server &serv, location &loc, std::string filename)
bool Config::parse_config_file(Server &serv, std::string filename)
{
	(void)serv;
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open file" << std::endl;
		return false;
	}
	if (file.is_open())
	{
		std::cout << "File opened successfully" << std::endl;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
		{
			Server serv;
			createServerr(file, serv);
			// std::cout << "Port: " << serv.getPort() << std::endl;
			// std::cout << "Server Name: " << serv.getServerName() << std::endl;
			// std::cout << "Root: " << serv.getRoot() << std::endl;
			// std::cout << "Index: " << serv.getIndex() << std::endl;
		}
	}
	// if (serv.getPort().empty() || serv.getServerName().empty() || serv.getRoot().empty())
	// {
	// 	generate_html_page_error(serv, 1, "403");
	// 	return false;
	// }
	// std::cout << "Port: " << serv.getPort() << std::endl;
	// std::cout << "Server Name: " << serv.getServerName() << std::endl;
	// std::cout << "Root: " << serv.getRoot() << std::endl;
	// std::cout << "Index: " << serv.getIndex() << std::endl;
	this->printConfig();
	return (true);
}



// bool Config::parse_config_file(Server &serv, std::string filename)
// {
// 	std::ifstream file(filename.c_str());
// 	if (!file.is_open())
// 	{
// 		std::cerr << "Error: could not open file" << std::endl;
// 		return false;
// 	}
// 	if (file.is_open())
// 	{
// 		std::cout << "File opened successfully" << std::endl;
// 	}
// 	std::string line;
// 	while (std::getline(file, line))
// 	{
// 		if (line.find("server") != std::string::npos)
// 			parse_listen(line, serv);
// 		if (line.find("listen") != std::string::npos)
// 			parse_listen(line, serv);
// 		if (line.find("server_name") != std::string::npos)
// 			parse_server_name(line, serv);
// 		if (line.find("index") != std::string::npos)
// 			parse_index(line, serv);
// 		if (line.find("root") != std::string::npos)
// 			parse_root(line, serv);
// 		if (line.find("error_page") != std::string::npos)
// 			parse_error_page(line, serv);
// 		if (line.find("location") != std::string::npos)
// 			parse_location(line, serv, file);
// 		std::cout << line << std::endl;
// 	}
// 	if (serv.getPort().empty() || serv.getServerName().empty() || serv.getRoot().empty())
// 	{
// 		generate_html_page_error(serv, 1, "403");
// 		return false;
// 	}
// 	return (true);
// }
