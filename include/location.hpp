#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Webserv.hpp"

class location
{
	private :
		std::string _index;
		std::string path;
	public :
		location();
		~location();
		location(const location &copy);
		location &operator=(const location &copy);

		//getter//
		std::string getIndex() const;
		std::string getPath() const;

		//setter//
		void setIndex(std::string index);
		void setPath(std::string path);
};

#endif