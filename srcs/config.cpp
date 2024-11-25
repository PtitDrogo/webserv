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
		std::cout << "	Error Page: " << it->first << " => " << it->second << std::endl;
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


size_t Config::addAllServers(std::vector<struct pollfd> &fds)
{
	size_t i;
	for (i = 0; i < _server.size(); i++)
	{
		int server_socket = SetupServerSocket(i);
		addPollFD(server_socket, fds);
	}
	return (i);
}

void	Config::addClient(int client_fd, int server)
{
	_clients[client_fd] = server;
}

int		Config::getIndexOfClientServer(int client_fd)
{
	return (_clients[client_fd]);
}

int Config::SetupServerSocket(int i)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Erreur lors de la configuration du socket." << std::endl;
		close(server_socket);
		return -1;
	}
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;


	int port = std::atoi(_server[i].getPort().c_str());

	// int port = std::atoi(conf.getServer()[0].getPort().c_str());
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Erreur lors de la liaison du socket." << std::endl;
		close(server_socket);
		return (-1);
	}
	if (listen(server_socket, 5) == -1)
	{
		std::cerr << "Erreur lors de la mise en écoute." << std::endl;
		close(server_socket);
		return -1;
	}
	return server_socket;
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
		std::cout << "}" << std::endl;
	}
}

void printVectorServer(std::vector<Server> serv)
{
	for (size_t i = 0; i < serv.size(); i++)
	{
		std::cout << "server { " << std::endl;
		std::cout << "	Port: " << serv[i].getPort() << std::endl;
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

void parse_listen(std::string line, Server &serv)
{
	std::cout << "parse_listen" << std::endl;
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("listen", pos) == pos)
	{
		size_t start = pos + 6;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string port = line.substr(start, end - start);
		if (!isdigit(port) || port.length() != 4)
		{
			std::cout << "Error: Invalid port number" << std::endl;
			return;
		}
		serv.setPort(port);
	}
}

void parse_server_name(std::string line, Server &serv)
{
	std::cout << "parse_server_name" << std::endl;	
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("server_name", pos) == pos)
	{
		size_t start = pos + 11;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string server_name = line.substr(start, end - start);
		if (server_name.empty() )
		{
			std::cout << "Error: Invalid server name" << std::endl;
			return;
		}
		serv.setServerName(server_name);
	}
}

void parse_index(std::string line, Server &serv)
{
	std::cout << "parse_index" << std::endl;
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
}

void parse_root(std::string line, Server &serv)
{
	std::cout << "parse_root" << std::endl;
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
}



void parse_error_page(std::string line, Server &serv)
{
	std::cout << "parse_error_page" << std::endl;
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
	std::cout << "parse_location" << std::endl;	
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
			}
			size_t rootPos = subLine.find("root");
			if (rootPos != std::string::npos) {
				size_t startRoot = subLine.find_first_not_of(" \t", rootPos + 4);
				size_t endRoot = subLine.find_first_of(" \t;", startRoot);
				std::string root = subLine.substr(startRoot, endRoot - startRoot);
				loc.setRoot(root);
			}
			size_t autoIndexPos = subLine.find("auto_index");
			if (autoIndexPos != std::string::npos) {
				size_t startAutoIndex = subLine.find_first_not_of(" \t", autoIndexPos + 10);
				size_t endAutoIndex = subLine.find_first_of(" \t;", startAutoIndex);
				std::string autoIndex = subLine.substr(startAutoIndex, endAutoIndex - startAutoIndex);
				if (autoIndex != "on" && autoIndex != "off") {
					std::cerr << "Error: invalid value for auto_index" << std::endl;
					return;
				}
				loc.setAutoIndex(autoIndex);
			}
			size_t allowMethodPos = subLine.find("allow_method");
			if (allowMethodPos != std::string::npos) {
				size_t startAllowMethod = subLine.find_first_not_of(" \t", allowMethodPos + 12); // 12 = length of "allow_method"
				size_t endAllowMethod = subLine.find_first_of("\n", startAllowMethod); 
				std::string allowMethod = subLine.substr(startAllowMethod, endAllowMethod - startAllowMethod);
				loc.setAllowMethod(allowMethod);
			}
			size_t cgiPathPos = subLine.find("cgi_path");
			if (cgiPathPos != std::string::npos) {
				size_t startCgiPath = subLine.find_first_not_of(" \t", cgiPathPos + 8);
				size_t endCgiPath = subLine.find_first_of(" \t;", startCgiPath);
				std::string cgiPath = subLine.substr(startCgiPath, endCgiPath - startCgiPath);
				loc.setCgiPath(cgiPath);
			}
		}
		serv.setLocation(loc);
	}
}

bool isdigit(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}
int convertToBytes(const std::string &size_str)
{
	std::string unit;
	int size = 0;

	size_t i = 0;
	while (i < size_str.size() && std::isdigit(size_str[i]))
	{
		size = size * 10 + (size_str[i] - '0');
		i++;
	}
	if (i < size_str.size())
		unit = size_str.substr(i);
	if (unit == "KB" || unit == "kb")
		return size * 1024;
	else if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	else if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;
	return size;
}

void parse_max_body_size(const std::string &line, Server &serv)
{
	std::cout << "parse_max_body_size" << std::endl;
	size_t pos = line.find_first_not_of(" \t");
	
	if (pos != std::string::npos && line.find("max_body_size", pos) == pos)
	{
		size_t start = pos + 13;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string max_body_size_str = line.substr(start, end - start);
		int max_body_size = convertToBytes(max_body_size_str);
		serv.setMaxBodySize(max_body_size);
		std::cout << "Max Body Size: " << serv.getMaxBodySize() << " bytes" << std::endl;
	}
}

void parse_auto_index(std::string line, Server &serv)
{
	std::cout << "parse_auto_index" << std::endl;
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("auto_index", pos) == pos)
	{
		size_t start = pos + 10;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string auto_index = line.substr(start, end - start);
		if (auto_index != "on" && auto_index != "off")
		{
			std::cerr << "Error: invalid value for auto_index" << std::endl;
			return;
		}
		serv.setAutoIndex(auto_index);
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
		if (line.find("max_body_size") != std::string::npos)
			parse_max_body_size(line, serv);
		if (line.find("error_page") != std::string::npos)
			parse_error_page(line, serv);
		if (line.find("location") != std::string::npos)
			parse_location(line, serv, file);
		if (line.find("auto_index") != std::string::npos)
			parse_auto_index(line, serv);
		if (line.find("}") != std::string::npos)
			break;
	}
	this->setServer(serv);
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

bool check_same_server_name(std::vector<Server> serv, size_t i, size_t j)
{
	std::cout << "check_same_server_name" << std::endl;
	if (serv[i].getServerName() == serv[j].getServerName())
	{
		std::cerr << "Error: same server name" << std::endl;
		return false;
	}
	return true;
}

bool check_same_port(std::vector<Server> serv)
{
	std::cout << "check_same_port" << std::endl;
	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t j = i + 1; j < serv.size(); j++)
		{
			std::cout << serv[i].getPort() << " " << serv[j].getPort() << std::endl;
			if (serv[i].getPort() == serv[j].getPort())
			{
				std::cout << serv[i].getPort() << " " << serv[j].getPort() << std::endl;
				if (check_same_server_name(serv, j , i) == false)
					return false;
			}
		}
	}
	return true;
}

bool Config::parse_config_file(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open file" << std::endl;
		return false;
	}
	if (file.is_open())
		std::cout << "File opened successfully" << std::endl;
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
		{
			Server serv;
			createServerr(file, serv);
		}
	}
	this->printConfig();
	if (!check_same_port(this->getServer()))
		return false;
	return (true);
}

