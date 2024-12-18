#include "server.hpp"

Server::Server()
{
	_max_body_size = -1;
}

Server::~Server() {}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server &Server::operator=(const Server &copy)
{
	if (this != &copy)
	{
		this->_index = copy._index;
		this->_port = copy._port;
		this->_root = copy._root;
		this->_server_name = copy._server_name;
		this->_return = copy._return;
		this->_error_page = copy._error_page;
		this->_location = copy._location;
		this->_max_body_size = copy._max_body_size;
		this->_auto_index = copy._auto_index;
		this->_host = copy._host;
		_server_socket = copy._server_socket;
		_cgi = copy._cgi;
	}
	return *this;
}

std::string Server::getPort() const { return (this->_port); }
std::string Server::getIndex() const { return (this->_index); }
std::string Server::getRoot() const { return (this->_root); }
std::string Server::getServerName() const { return (this->_server_name); }
std::map<std::string, std::string> Server::getReturn() const { return (this->_return); }
std::vector<location> &Server::getLocation() { return (this->_location); }
std::map<std::string, std::string> Server::getErrorPage() const { return (this->_error_page); }
int Server::getMaxBodySize() const { return (this->_max_body_size); }
std::string Server::getAutoIndex() const { return (this->_auto_index); }
std::string Server::getHost() const { return (this->_host); }
int Server::getServerSocket() const { return (_server_socket); }
std::map<std::string, std::string>& Server::getCgis() { return (_cgi);}

void Server::setPort(std::string port) { this->_port = port; }
void Server::setIndex(std::string index) { this->_index = index; }
void Server::setRoot(std::string root) { this->_root = root; }
void Server::setServerName(std::string server_name) { this->_server_name = server_name; }
void Server::setReturn(std::map<std::string, std::string> return_) { this->_return = return_; }
void Server::setErrorPage(std::string &error_code, std::string &error_file) { _error_page[error_code] = error_file; }
void Server::setLocation(location &loc) { this->_location.push_back(loc); }
void Server::setMaxBodySize(int max_body_size) { this->_max_body_size = max_body_size; }
void Server::setAutoIndex(std::string auto_index) { this->_auto_index = auto_index; }
void Server::setServerSocket(int server_socket) { _server_socket = server_socket; }
void Server::setHost(std::string host) { this->_host = host; }

void Server::addCgis(std::string& extension ,std::string& interpreter_path)
{
	_cgi[extension] = interpreter_path;
}


int	execute_server(Config& conf, char **envp)
{
	Cookies cook;
	std::vector<struct pollfd> fds;

	size_t number_of_servers = conf.addAllServers(fds);

	while (Config::ServerRunning)
	{
		signal(SIGINT, &handleSignal);
    	signal(SIGTERM, &handleSignal);
		checkIfNewClient(fds, number_of_servers, conf);
		if (safe_poll(fds, number_of_servers) == FAILURE)
			return FAILURE;
		for (size_t i = number_of_servers; i < fds.size(); ++i)
		{
			Client &client = conf.getClientObject(fds[i].fd);
			HttpRequest req;
			if (fds[i].revents & POLLRDHUP || fds[i].revents & POLLHUP)
			{
				if (checkFailedExecve(client) == false)
					std::cerr << "Error sending back that execve failed" << std::endl;
				disconnectClient(fds, client, conf);
				break;
			}
			if (fds[i].revents & POLLERR)
			{
				disconnectClient(fds, client, conf);
				break;
			}
			if (handleTimeout(client, fds, conf, i) == true)
				continue ;
			if ((!(fds[i].revents & POLLIN)))
				continue;
			if (isCgiStuff(client, conf, fds, i) == true)
				continue ;
			// Lecture initiale du buffer
			char buffer[4096] = {0};
			int recv_value = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (handleRecvValue(recv_value) == FAILURE)
			{	
				disconnectClient(fds, client, conf);
				break ;
			}
			client.appendToRequest(buffer, recv_value);

			if (client.getTotalRead() >= client.getContentLength()) 
			{
				std::string type_request = get_type_request(client.getRequest(), req);
				if (type_request == "POST")
				{
					if (preparePostParse(client, cook, req) == false)
					{	
						disconnectClient(fds, client, conf);;
						break ;
					}
				}
				else if (type_request == "GET") 
				{
					if (prepareGetParse(client, cook, req) == false) 
					{	
						disconnectClient(fds, client, conf);;
						break ;
					}
				}
				else if (type_request == "DELETE")
				{	
					if (parse_buffer_delete(client) == false)
					{	
						disconnectClient(fds, client, conf);;
						break ;
					}
				}
				else if (type_request == "CGI-GET" || type_request == "CGI-POST")
					cgiProtocol(envp, req, client, conf, fds);
				else
				{
					if (generate_html_page_error(client, "400") == false)
					{	
						disconnectClient(fds, client, conf);;
						break ;
					}
				}
				client.reset();
			}
		}
	}
	return SUCCESS;
}

void handleSignal(int signum) 
{
	static_cast<void>(signum);
	std::cout  << std::endl << MAGENTA << "Shutting Down Server, Bye !"  << RESET << std::endl;
	Config::ServerRunning = false;
	signal(SIGINT, &handleSignal);
	signal(SIGTERM, &handleSignal);
}