#include "config.hpp"

bool Config::ServerRunning = true;

Config::Config() {
}

Config::~Config() {}

Config::Config(const Config &copy) 
{
	*this = copy;
}

Config &Config::operator=(const Config &copy)
{
	if (this != &copy)
	{
		this->_servers = copy._servers;
		this->_clients = copy._clients;
	}
	return *this;
}

std::vector<Server> &Config::getServer()
{
	return this->_servers;
}

void Config::setServer(Server &serv)
{
	this->_servers.push_back(serv);
}

size_t Config::addAllServers(std::vector<struct pollfd> &fds)
{
	size_t i;
	for (i = 0; i < _servers.size(); i++)
	{
		int server_socket = SetupServerSocket(i);
		_servers[i].setServerSocket(server_socket);
		addPollFD(server_socket, fds);
	}
	return (i);
}

void	Config::addClient(int client_fd, Server &serv)
{
	_clients.insert(std::map<int, Client>::value_type(client_fd, Client(client_fd, serv)));
}

Server &Config::getServerOfClient(int client_fd)
{
	std::map<int, Client>::const_iterator it = _clients.find(client_fd);
	if (it != _clients.end()) {
		return it->second.getServer();
	}
	throw std::out_of_range("Somehow, your socket doesnt have a server");
}

Client &Config::getClientObject(int client_fd)
{
	std::map<int, Client>::iterator it = _clients.find(client_fd);
	if (it != _clients.end()) {
		return it->second;
	}
	throw std::out_of_range("Somehow, your socket isnt linked to a client object");
}

std::map<int, Client>& Config::getClientsMap()
{
	return(_clients);
}

void Config::removeClient(int client_fd) 
{
    _clients.erase(client_fd);
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


	int port = std::atoi(_servers[i].getPort().c_str());

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


bool Config::createServerr(std::ifstream &file , Server &serv)
{
	std::string line;

	while(std::getline(file, line))
	{
		if (isCommentLine(line) == true)
			continue ;
		if (line.find("listen") != std::string::npos)
			if (parse_listen(line, serv) == false)
				return false;
		if (line.find("server_name") != std::string::npos)
			if (parse_server_name(line, serv) == false)
				return false;
		if (line.find("index") != std::string::npos)
			parse_index(line, serv);
		if (line.find("root") != std::string::npos)
			parse_root(line, serv);
		if (line.find("max_body_size") != std::string::npos)
			parse_max_body_size(line, serv);
		if (line.find("error_page") != std::string::npos)
			parse_error_page(line, serv);
		if (line.find("location") != std::string::npos)
			if (parse_location(line, serv, file) == false)
				return false;
		if (line.find("auto_index") != std::string::npos)
			parse_auto_index(line, serv);
		if (line.find("cgi_path") != std::string::npos)
			parse_cgi_path(line, serv);
		if (line.find("}") != std::string::npos)
			break;
	}
	this->setServer(serv);
	return true;
}


void Config::printConfig(std::ifstream& file)
{
	std::vector<Server> serv = this->getServer();
	std::string line;
	while (std::getline(file, line))
	{
		std::cout << BLUE << line << RESET << std::endl;
	}
}

bool check_same_server_name(std::vector<Server> serv, size_t i, size_t j)
{
	if (serv[i].getServerName() == serv[j].getServerName())
	{
		std::cout << "Webserv : conflicting server name" << std::endl;
		return false;
	}
	return true;
}

bool check_same_port(std::vector<Server> serv)
{
	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t j = i + 1; j < serv.size(); j++)
		{
			if (serv[i].getPort() == serv[j].getPort())
			{
				if (check_same_server_name(serv, j , i) == false)
					return false;
			}
		}
	}
	return true;
}

bool check_same_path_of_location(const std::vector<location>& loc, size_t i, size_t j)
{
	if (loc.size() <= std::max(i, j))
	{
		std::cerr << "Webserv : Not enough locations to compare paths" << std::endl;
		return true;
	}
	if (loc[i].getPath() == loc[j].getPath())
	{
		std::cerr << "Webserv : Conflicting paths in locations" << std::endl;
		return false;
	}
	return true;
}

bool Config::parse_config_file(std::string filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return false;
	if (file.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "Error: file is empty" << std::endl;
		return false;
	}
	if (count_bracket(file) == false)
		return false;
	file.clear();
	file.seekg(0, std::ios::beg);
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
		{
			Server serv;
			if (createServerr(file, serv) == false)
				return false;
		}
	}
	file.clear();
	file.seekg(0, std::ios::beg);
	// this->printConfig(file);
	if (!check_same_port(this->getServer()))
		return false;
	//tfreydie, added checking accross all servers;
	for (size_t x = 0; x < _servers.size(); x++)
	{
		std::vector<location> locs = _servers[x].getLocation();
		for (size_t i = 0; i < locs.size(); ++i)
		{
			for (size_t j = i + 1; j < locs.size(); ++j)
			{
				if (!check_same_path_of_location(locs, i, j))
					return false;
			}
		}
	}

	return (true);
}
