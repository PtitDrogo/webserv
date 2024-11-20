#include "Webserv.hpp"

//GET /config/cgi-bin/time.py HTTP/1.1
//GET /config/cgi-bin/helloworld.py HTTP/1.1
//GET /favicon.ico HTTP/1.1 is a typical request
//GET /hello-world.py would be a cgi request;

/*
Step to do a CGI:
- Recognize that the end of the request is a CGI worthy file (.py, .php)
- Then we Send a response and execute the cgi using fork and dup
- We add everything we can from the request to the env of the cgi. (TODO)
- ARGV is ideally = argv[0] : interpreter path, argv[1] = cgi_path, argv[2] = NULL
(a bash script could add flags somehow).
- We execute the cgi.
- We read cgi_tmp/webserv_cgi_stdout and send the content back to the client
*/


CgiHandler::CgiHandler(char * const *envp) :
_envp(envp),
_argv(NULL),
_exit_status(-1),
_path(""),
_pid(-1)
{}

CgiHandler::~CgiHandler() {}


void CgiHandler::HandleCgiRequest(const HttpRequest &request)
{
    
    //A LOT OF PARSING WILL HAPPEN HERE TO SPLIT PATH INTO EXE AND PARAMETERS
    _path = "." + request.getPath();
    std::cout << "hi whats up cgi handler here path is |" << _path << "|" << std::endl;

    //Execute the fucking cgi;
    int PID = fork();
    if (PID == 0)
    {
        //Modern dup    
        std::freopen(PATH_CGI_IN, "r", stdin);
		std::freopen(PATH_CGI_OUT, "w", stdout);
		std::freopen(PATH_CGI_ERR, "w", stderr);
        execve(_path.c_str(), _argv, _envp);
        std::cerr << "I failed to execve :(" << std::endl;
        std::cerr << "failed to execve, path was : " << _path << std::endl;
        perror("execve");
        std::exit(EXIT_FAILURE);
    }
    if (PID > 0)
    {
        int status;
        waitpid(PID, &status, 0); //TODO: change this so server doesnt get blocked
    }
    else if (PID < 0)
    {
        std::cout << "Fork failed" << std::endl;
		std::exit(EXIT_FAILURE); //tmp, just to be sure
        //this will be an error 500 !
    }
}

void    cgiProtocol(char *const *envp, const HttpRequest &request)
{
    CgiHandler cgi(envp);

    cgi.HandleCgiRequest(request);

}




