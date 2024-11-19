#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "Webserv.hpp"

class Config
{
	private :
		std::vector<Server> _server;
		
	public:
		Config();
		~Config();
		Config(const Config &copy);
		Config &operator=(const Config &copy);
		bool parse_config_file(Server &serv, std::string filename);
		// bool parse_config_file(std::string filename);

		// bool parse_config_file(Server &serv, location &loc, std::string filename);
		std::vector<Server> &getServer();
		void setServer(Server &serv);

		void createServerr(std::ifstream &file , Server &serv);
		void createServer(std::string line, std::ifstream &file);
		void printConfig();
};

#endif
