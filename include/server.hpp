/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 11:15:25 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/08 15:15:36 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "include.hpp"

class Server
{
	private :
		std::string _port;
		std::string _index;
		std::string _server_name;
		std::string _root;
		std::map<std::string , std::string> _return;
		std::map<std::string , std::string> _error_page;
	public :
		Server();
		~Server();
		Server(const Server &copy);
		Server &operator=(const Server &copy);
		std::string getPort() const;
		std::string getIndex() const;
		std::string getRoot() const;
		std::string getServerName() const;
		std::map<std::string , std::string> getReturn() const;
		std::map<std::string, std::string> getErrorPage() const;

		//setter//

		void setPort(std::string port);
		void setIndex(std::string index);
		void setRoot(std::string root);
		void setServerName(std::string server_name);
		void setReturn(std::map<std::string , std::string> return_);
		void setErrorPage(std::string& error_code,std::string& error_file);
};

#endif