#ifndef WEBSERV_HPP
# define WEBSERV_HPP


//*********************************************************//
//************************INCLUDES*************************//
//*********************************************************//


#include "include.hpp"
#include "server.hpp"


//*********************************************************//
//*************************DEFINES*************************//
//*********************************************************//


# define SUCCESS 0
# define FAILURE 1


//*********************************************************//
//************************FUNCTIONS************************//
//*********************************************************//


//-----------ParseBuffer-----------//
void	parse_buffer_get(std::string buffer, Server &serv , int client_socket);
void	parse_buffer_post(std::string buffer , int client_socket, Server &serv);

//-----------SetUpSocket-----------//
int SetupSocket(Server serv);
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
void 		generate_html_page_error(Server &serv, int client_socket, std::string error_code);

//-----------CGI-----------//
void cgiHandler(char **envp);

#endif