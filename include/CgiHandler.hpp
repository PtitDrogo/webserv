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
    pid_t _pid;
    CgiHandler(); //have to build with request

public:
    ~CgiHandler();
    CgiHandler(char *const *envp);
    void HandleCgiRequest(const std::string &request); 
    //Request should contain all the info i would hope
};