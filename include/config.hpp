/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:15:40 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/06 18:03:13 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "include.hpp"

class Config
{
	private :
		std::vector<int> _ports;
		std::vector<std::string> _server_names;
		std::map<std::string, std::string> _url_redirection;
		
	public:
		Config();
		~Config();
		Config(const Config &copy);
		Config &operator=(const Config &copy);
		bool parse_config_file();
		std::vector<int> get_ports() const;
		std::vector<std::string> get_server_names() const;
		std::map<std::string, std::string> get_url_redirection() const;
};

#endif