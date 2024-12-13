#ifndef WEBSERV_HPP
# define WEBSERV_HPP



//*********************************************************//
//*************************CLASS***************************//
//*********************************************************//

class location;
class HttpRequest;
class Config;
class Server;
class Client;
class HttpRequestParser;
class ClientUploadState;
class Cookies;

//*********************************************************//
//************************INCLUDES*************************//
//*********************************************************//

#include "include.hpp"
#include "server.hpp"
#include "config.hpp"
#include "httpRequest.hpp"
#include "Cookies.hpp"

//*********************************************************//
//*************************DEFINES*************************//
//*********************************************************//

# define SUCCESS 0
# define FAILURE 1
# define POLL_TIMEOUT_MILISECONDS 5000
# define CGI_TIMEOUT_SECONDS 12
# define PIPE_BUFFER 65535


//*********************************************************//
//*************************COLORS***************************//
//*********************************************************//

#define RESET	"\033[0m"
#define MAGENTA	"\033[35m"
#define YELLOW "\033[33m"
#define BLUE "\033[1;34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define NLINE std::cout << std::endl;


//*********************************************************//
//************************FUNCTIONS************************//
//*********************************************************//

//-----------ParseRequests-----------//
std::string parse_request(std::string type, std::string buffer, HttpRequest &req);
std::string get_type_request(std::string buffer, HttpRequest &req);
bool isCgiRequest(const HttpRequest &req);

//-----------ParseBuffer-----------//
// void	parse_buffer_get(std::string buffer, Config &conf , int client_socket);
void	parse_buffer_get(Client &client, HttpRequest &req);
void	parse_buffer_post(Client& client, Cookies &cook);
bool    preparePostParse(Client& client, Cookies &cook);
bool    prepareGetParse(Client& client, HttpRequest &req);


//-----------SetUpSocket-----------//
int SetupClientAddress(int server_socket);


//-----------HandleClients-----------//
void    checkIfNewClient(std::vector<struct pollfd> &fds, size_t number_of_servers, Config &conf);
int     safe_poll(std::vector<struct pollfd> &fds, size_t number_of_servers);
int     handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds, Config& conf);
void    addPollFD(int client_socket, std::vector<struct pollfd> &fds);
// void    disconnectClient(std::vector<struct pollfd> &fds, size_t &i, Config& conf);
void    disconnectClient(std::vector<struct pollfd> &fds, Client& client, Config& conf);
bool	handleTimeout(Client& client, std::vector<struct pollfd> &fds, Config& conf, size_t &i);



//-----------Server-----------//
std::string readFile(std::string &path);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
void 		generate_html_page_error(const Client& client, std::string error_code);

//-----------Delete-----------//

bool deleteFile(const std::string& path);
void parse_buffer_delete(std::string buffer, Client& client);

//-----------CGI-----------//
void    cgiProtocol(char *const *envp, const HttpRequest &request, Client& client, Config &conf, std::vector<struct pollfd> &fds);
bool    isCgiStuff(Client& client, Config &conf, std::vector<struct pollfd> &fds, size_t i);

//-----------Utils-----------//
std::string fileToString(const char *filePath);
std::string intToString(int value);
std::string readFromPipeFd(int pipefd);
bool        isRegularFile(const std::string& path);


//-----------DEBUG-PRINTS-----------//
void printVectorloc2(std::vector<location> loc);
void printVectorServer2(std::vector<Server> serv);

std::string handleAutoIndex(const std::string& path);
std::string generateAutoIndexPage(const std::string& directory, const std::vector<std::string>& files, Client& client);
std::vector<std::string> listDirectory(const std::string& directory);

//-----------utils-----------//

bool isdigit(std::string str);
bool isExtension(std::string path);
std::string trim(const std::string& str);


#endif