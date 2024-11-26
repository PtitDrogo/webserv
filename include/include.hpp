#ifndef INCLUDE_HPP
# define INCLUDE_HPP


//-----------ExternalLibraries-----------//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iterator>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>


//-----------InternalLibraries-----------//
#include "location.hpp"
#include "print.tpp"
#include "config.hpp"
#include "Webserv.hpp"
#include "server.hpp"
#include "httpRequest.hpp"
#include "CgiHandler.hpp"

//llitot classes
#include "HttpRequestParser.hpp"
#include "ClientUploadState.hpp" 
#include "Client.hpp"

#endif