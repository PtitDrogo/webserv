#ifndef WEBSERV_HPP
# define WEBSERV_HPP


//*********************************************************//
//************************INCLUDES*************************//
//*********************************************************//


#include "include.hpp"
#include "server.hpp"
#include "config.hpp"


//*********************************************************//
//*************************DEFINES*************************//
//*********************************************************//


# define SUCCESS 0
# define FAILURE 1


//*********************************************************//
//************************FUNCTIONS************************//
//*********************************************************//


class Config;

//-----------ParseBuffer-----------//
void	parse_buffer_get(std::string buffer, Config &conf , int client_socket);
void	parse_buffer_post(std::string buffer , int client_socket, Config &conf);

//-----------SetUpSocket-----------//
int SetupSocket(Server serv, Config conf);
int SetupClientAddress(int server_socket);


//-----------HandleClients-----------//
void    checkIfNewClient(std::vector<struct pollfd> &fds, int server_socket);
int     safe_poll(std::vector<struct pollfd> &fds, int server_socket);
int     handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds);
void    addPollFD(int client_socket, std::vector<struct pollfd> &fds);
void    disconnectClient(std::vector<struct pollfd> &fds, size_t &i);



//-----------Server-----------//
std::string readFile(std::string &path);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
void 		generate_html_page_error(Config &conf, int client_socket, std::string error_code);

//-----------Delete-----------//

bool deleteFile(const std::string& path);
void parse_buffer_delete(std::string buffer, int client_socket, Config &conf);

//-----------CGI-----------//
void cgiHandler(char **envp);

#endif