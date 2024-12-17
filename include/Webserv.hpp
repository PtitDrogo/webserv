#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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

#define SUCCESS 0
#define FAILURE 1
#define POLL_TIMEOUT_MILISECONDS 5
#define CGI_TIMEOUT_SECONDS 12
#define PIPE_BUFFER 65535
#define EXECVE_FAILURE 127

//*********************************************************//
//*************************COLORS***************************//
//*********************************************************//

#define RESET "\033[0m"
#define MAGENTA "\033[1;35m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
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
void parse_buffer_get(Client &client, Cookies &cook, HttpRequest &req);
void parse_buffer_post(Client &client, Cookies &cook, HttpRequest &req);
bool preparePostParse(Client &client, Cookies &cook, HttpRequest &req);
bool prepareGetParse(Client &client, Cookies &cook, HttpRequest &req);

//-----------SetUpSocket-----------//
int SetupClientAddress(int server_socket);

//-----------HandleClients-----------//
void checkIfNewClient(std::vector<struct pollfd> &fds, size_t number_of_servers, Config &conf);
int safe_poll(std::vector<struct pollfd> &fds, size_t number_of_servers);
int handleRecvValue(int valread);
void addPollFD(int client_socket, std::vector<struct pollfd> &fds);
void disconnectClient(std::vector<struct pollfd> &fds, Client &client, Config &conf);
bool handleTimeout(Client &client, std::vector<struct pollfd> &fds, Config &conf, size_t &i);

//-----------Server-----------//
std::string readFile(std::string &path);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
void generate_html_page_error(const Client &client, std::string error_code);
bool file_exists_parsebuffer(const char *path);

//-----------Delete-----------//

bool deleteFile(const std::string &path);
void parse_buffer_delete(std::string buffer, Client &client);

//-----------Download-----------//
bool download(Client& client);

//-----------Upload-----------//
bool upload(Client& client);


//-----------CGI-----------//
void cgiProtocol(char *const *envp, const HttpRequest &request, Client &client, Config &conf, std::vector<struct pollfd> &fds);
bool isCgiStuff(Client &client, Config &conf, std::vector<struct pollfd> &fds, size_t i);

//-----------Utils-----------//
std::string fileToString(const char *filePath);
std::string readFromPipeFd(int pipefd);
bool isRegularFile(const std::string &path);

//-----------DEBUG-PRINTS-----------//
void printVectorloc2(std::vector<location> loc);
void printVectorServer2(std::vector<Server> serv);

std::string handleAutoIndex(const std::string &path);
std::string generateAutoIndexPage(const std::string &directory, const std::vector<std::string> &files, Client &client);
std::vector<std::string> listDirectory(const std::string &directory);

//-----------utils-----------//

bool isdigit(std::string str);
bool isExtension(std::string path);
std::string trim(const std::string& str);
std::string injectUserHtml(const std::string& fileContent, const std::string& username);
bool isCommentLine(const std::string line);
bool count_bracket(std::ifstream &file);


//-----------ParseServer-----------//

bool parse_listen(std::string line, Server &serv);
bool parse_server_name(std::string line, Server &serv);
void parse_index(std::string line, Server &serv);
void parse_root(std::string line, Server &serv);
void parse_error_page(std::string line, Server &serv);
void parse_max_body_size(const std::string &line, Server &serv);
void parse_auto_index(std::string line, Server &serv);
void parse_cgi_path(std::string& line, Server &server);

//-----------ParseLocation-----------//

bool parse_location(std::string line, Server &serv, std::ifstream &file);
std::string parse_no_location(std::string path, Client &client, std::string finalPath, int client_socket);
bool isMethodAllowed(const std::string& allowedMethods, const std::string& reqMethod);
std::string parse_with_location(Client &client, std::string finalPath, HttpRequest &req);
std::string CheckLocation(const std::string& path, std::vector<location>& locationPath, Client& client);
std::string parse_no_location(std::string path, Client &client, std::string finalPath, int client_socket);
std::string parse_with_location(Client &client, std::string finalPath, HttpRequest &req);

//-----------Print-----------//

void printVector(std::map<std::string, std::string> errorPage);
void printMapRedirect(std::map<std::string, std::string> redirect);
void printVectorloc(std::vector<location> loc);
void printVectorServer(std::vector<Server> serv);
void printMapCgi(std::map<std::string, std::string> cgi);

//-----------Cookies-----------//

std::string handle_connexion(std::string username, std::string password, Cookies &cook, std::string request_token, Client& client);
std::string handle_deconnexion(Cookies &cook, std::string request_token, Client& client);

//-----------Auto_index-----------//

void autoIndex(std::string path, Client& client);

//-----------Check-----------//

bool check_host(std::string line, const Server& Server);
void checkFailedExecve(Client &client);

#endif