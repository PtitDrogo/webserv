#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Webserv.hpp"

class location
{
	private :
		std::string _index;
		std::string path;
		std::string _root;
		std::string _auto_index;
		std::string _allow_method;
		std::string _cgi_path;
		std::map<std::string, std::string> _redir;
		std::map<std::string, std::string> _cgi;

	public :
		location();
		~location();
		location(const location &copy);
		location &operator=(const location &copy);

		//getter//
		std::string getIndex() const;
		std::string getPath() const;
		std::string getRoot() const;
		std::string getAutoIndex() const;
		std::string getAllowMethod() const;
		std::string getCgiPath() const;
		std::map<std::string, std::string> getRedir() const;
		bool getCanPost() const;
		bool getCanDelete() const;
		std::map<std::string, std::string> getCgi() const;



		//setter//
		void setIndex(std::string index);
		void setPath(std::string path);
		void setRoot(std::string root);
		void setAutoIndex(std::string auto_index);
		void setAllowMethod(std::string allow_method);
		void setCgiPath(std::string cgi_path);
		void setRedir(std::string& error_code, std::string& path);
		void setCanPost(bool canPost);
		void setCanDelete(bool canDelete);
		void setCgi(std::string& cgi_path, std::string& cgi_extension);
};

#endif