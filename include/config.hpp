#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "Webserv.hpp"

class Config
{
	private :
		std::vector<Server>        _servers;
		std::map<int, Client>      _clients; //Link fd to their Client class associated (the server of the client will be inside that class)
		bool islocation;
	public:
		Config();
		~Config();
		Config(const Config &copy);
		Config &operator=(const Config &copy);
		bool parse_config_file(std::string filename);

		int		SetupServerSocket(int i);
		size_t	addAllServers(std::vector<struct pollfd> &fds);
		void	addClient(int client_fd, Server &serv);

		// bool parse_config_file(std::string filename);
		Server &getServerOfClient(int client_fd);
		Client &getClientObject(int client_fd);
		std::map<int, Client>& getClientsMap();
		std::vector<Server> &getServer();

		// bool parse_config_file(Server &serv, location &loc, std::string filename);
		void setServer(Server &serv);
		bool getIsLocation();
		void removeClient(int client_fd);

		bool createServerr(std::ifstream &file , Server &serv);
		void createServer(std::string line, std::ifstream &file);
		void printConfig(std::ifstream& file);
		void setIsLocation(bool islocation);
};

#endif