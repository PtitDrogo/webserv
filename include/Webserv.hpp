#ifndef WEBSERV_HPP
# define WEBSERV_HPP



//*********************************************************//
//*************************CLASS***************************//
//*********************************************************//

class location;
class Config;
class HttpRequest;
class Server;


//*********************************************************//
//************************INCLUDES*************************//
//*********************************************************//

#include "include.hpp"
#include "server.hpp"
#include "config.hpp"
#include "httpRequest.hpp"

//*********************************************************//
//*************************DEFINES*************************//
//*********************************************************//

# define SUCCESS 0
# define FAILURE 1
# define POLL_TIMEOUT 5000

//*********************************************************//
//************************FUNCTIONS************************//
//*********************************************************//

//-----------ParseRequests-----------//
std::string parse_request(std::string type, std::string buffer, HttpRequest &req);
std::string get_type_request(std::string buffer, HttpRequest &req);
bool isCgiRequest(const HttpRequest &req);


//-----------ParseBuffer-----------//
// void	parse_buffer_get(std::string buffer, Config &conf , int client_socket);
void	parse_buffer_get(std::string buffer, Config &conf , int client_socket, HttpRequest &req);
void	parse_buffer_post(std::string buffer , int client_socket, Config &conf);
bool    preparePostParse(int fd, char *buffer, Config &conf, int recv_value);

//-----------SetUpSocket-----------//
int SetupClientAddress(int server_socket);


//-----------HandleClients-----------//
void    checkIfNewClient(std::vector<struct pollfd> &fds, size_t number_of_servers, Config &conf);
int     safe_poll(std::vector<struct pollfd> &fds, size_t number_of_servers);
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
void    cgiProtocol(char *const *envp, const HttpRequest &request, int fd_client);


//-----------Utils-----------//
std::string fileToString(const char *filePath);


//-----------DEBUG-PRINTS-----------//
void printVectorloc2(std::vector<location> loc);
void printVectorServer2(std::vector<Server> serv);

std::string handleAutoIndex(const std::string& path);
std::string generateAutoIndexPage(const std::string& directory, const std::vector<std::string>& files);
std::vector<std::string> listDirectory(const std::string& directory);

//-----------utils-----------//

bool isdigit(std::string str);
bool isExtension(std::string path);
std::string trim(const std::string& str);


#endif