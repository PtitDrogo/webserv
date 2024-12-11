#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Webserv.hpp"

class CgiHandler
{
private:
    // std::map<std::string, std::string>	_env; //what is this ?
    char *const *   _envp;
    char **         _argv;
    int             _exit_status;
    std::string     _path;
    pid_t           _pid;
    int             _pipe_in[2];
    int             _pipe_out[2];
    Client&         _client; //client calling the cgi;
    std::map<std::string, std::string> _params;
    CgiHandler(); //have to build with request
    char **  updateEnv();
    pid_t    executeCGI();
    void	 processCgiPath();
    // void     freeUpdatedEnv(char * const *tofree);

public:
    ~CgiHandler();
    CgiHandler(char *const *envp, Client& client);
    bool     HandleCgiRequest(const HttpRequest &request); 

    //getters
    int     *getPipeOut();
    int     *getPipeIn();
    pid_t   getPID();
};

#endif
