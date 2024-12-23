#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class Server
{
private:
	std::string _port;
	std::string _index;
	std::string _server_name;
	std::string _root;
	std::map<std::string, std::string> _return;
	std::map<std::string, std::string> _error_page;
	std::vector<location> _location;
	int _max_body_size;
	std::string _auto_index;
	std::string _host;
	int _server_socket;
	std::map<std::string, std::string> _cgi;

public:
	Server();
	~Server();
	Server(const Server &copy);
	Server &operator=(const Server &copy);

	// getter//

	std::string getPort() const;
	std::string getIndex() const;
	std::string getRoot() const;
	std::string getServerName() const;
	std::map<std::string, std::string> getReturn() const;
	std::map<std::string, std::string> getErrorPage() const;
	int getMaxBodySize() const;
	std::vector<location> &getLocation();
	std::string getAutoIndex() const;
	int getServerSocket() const;
	// std::string getLocation() const;
	std::string getHost() const;
	std::map<std::string, std::string>& getCgis();

	// setter//

	void setPort(std::string port);
	void setIndex(std::string index);
	void setRoot(std::string root);
	void setServerName(std::string server_name);
	void setReturn(std::map<std::string, std::string> return_);
	void setErrorPage(std::string &error_code, std::string &error_file);
	void setLocation(location &loc);
	void setMaxBodySize(int max_body_size);
	void setAutoIndex(std::string auto_index);
	void setHost(std::string host);
	void setServerSocket(int server_socket);
	void addCgis(std::string& extension ,std::string& interpreter_path);
};

#endif