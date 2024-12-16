#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Webserv.hpp"


class CgiHandler
{
private:
    char *const *   _envp;
    char **         _argv;
    int             _exit_status;
    std::string     _path;
    pid_t           _pid;
    int             _pipe_in[2];
    int             _pipe_out[2];
    Client&         _client; //client calling the cgi;
    std::map<std::string, std::string> _params;
    std::string     _body_post;
    std::string     _interpreter;

    CgiHandler(); //have to build with request
    char **  updateEnv();
    pid_t    executeCGI(const HttpRequest &request);
    void	 processCgiPath(const HttpRequest &request);
    void     freeUpdatedEnv(char **tofree);

public:
    ~CgiHandler();
    CgiHandler(char *const *envp, Client& client);
    bool     HandleCgiRequest(Client& client, const HttpRequest &request); 

    //getters
    int     *getPipeOut();
    int     *getPipeIn();
    pid_t   getPID();
};

#endif
