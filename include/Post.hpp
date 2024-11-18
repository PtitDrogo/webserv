#ifndef POST_HPP
# define POST_HPP


#include "Webserv.hpp"

class Post
{
	public:
		Post();
		~Post();
		Post(const Post &copy);
		Post &operator=(const Post &copy);
};

#endif