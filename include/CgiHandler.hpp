#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Webserv.hpp"

# define PATH_CGI_OUT "./cgi_tmp/webserv_cgi_stdout"
# define PATH_CGI_ERR "./cgi_tmp/webserv_cgi_stderr"
# define PATH_CGI_IN "./cgi_tmp/webserv_cgi_stdin"



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

public:
    ~CgiHandler();
    CgiHandler(char *const *envp, Client& client);
    pid_t    executeCGI();
    bool     HandleCgiRequest(const HttpRequest &request); 
    void	processCgiPath();

    //getters
    int     *getPipeOut();
    int     *getPipeIn();
    pid_t   getPID();
};

#endif
