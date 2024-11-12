#ifndef PARSE_BUFFER_HPP
# define PARSE_BUFFER_HPP

#include "include.hpp"

int parse_buffer(std::string buffer, location &loc);
void generateResponse(Server &serv, std::string buffer, location &loc, int client_socket);


#endif
