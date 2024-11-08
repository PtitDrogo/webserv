/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:15:40 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/08 13:25:29 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "include.hpp"
#include "server.hpp"
#include <vector>        // Pour std::vector
#include <string>        // Pour std::string
#include <map>   

class Server;

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
		std::vector<Server> getServer() const;
};

#endif
