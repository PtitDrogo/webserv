#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "Webserv.hpp"

class Config
{
	private :
		std::vector<Server>    _server;
		std::map<int, int>  _clients; //Link each clients fd to their server index
		
	public:
		Config();
		~Config();
		Config(const Config &copy);
		Config &operator=(const Config &copy);
		bool parse_config_file(std::string filename);

		int		SetupServerSocket(int i);
		size_t	addAllServers(std::vector<struct pollfd> &fds);
		void	addClient(int client_fd, int);

		// bool parse_config_file(std::string filename);
		int		getIndexOfClientServer(int client_fd);

		// bool parse_config_file(Server &serv, location &loc, std::string filename);
		std::vector<Server> &getServer();
		void setServer(Server &serv);

		void createServerr(std::ifstream &file , Server &serv);
		void createServer(std::string line, std::ifstream &file);
		void printConfig();
};

#endif
