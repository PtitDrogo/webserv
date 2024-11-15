#include "Webserv.hpp"
#include "CgiHandler.hpp"
//GET /favicon.ico HTTP/1.1 is a typical request
//GET /hello-world.py would be a cgi request;

/*
Step to do a CGI:
- Recognize that the end of the request is a CGI worthy file (.py, .php)
- Then we Send a response and execute the cgi using fork and dup
(Question: Do we execute the cgi then send the response ??)
*/


CgiHandler::CgiHandler(char * const *envp) :
_envp(envp),
_argv(NULL),
_exit_status(-1),
_path(""),
_pid(-1)
{}

CgiHandler::~CgiHandler() {}


void CgiHandler::HandleCgiRequest(const std::string& request)
{
    std::cout << "hi whats up cgi handler here" << std::endl;
    _path = "./config/cgi-bin/" + request;

    //Execute the fucking cgi;
    int PID = fork();
    if (PID == 0)
    {
        //Modern dup    
        std::freopen(PATH_CGI_IN, "r", stdin);
		std::freopen(PATH_CGI_OUT, "w", stdout);
		std::freopen(PATH_CGI_ERR, "w", stderr);
        execve(_path.c_str(), _argv, _envp);
        std::cout << "I failed to execve :(" << std::endl;
        perror("execve");
        std::exit(EXIT_FAILURE);
    }
    if (PID > 0)
    {
        //We do nothing in the parent i guess
    }
    else if (PID < 0)
    {
        std::cout << "Fork failed" << std::endl;
		//this will be an error 500 !
    }
}

void    cgiProtocol(char *const *envp, const std::string& request)
{
    CgiHandler cgi(envp);

    cgi.HandleCgiRequest(request);

    
    //SO TECHNICALLY SINCE I HAVE A CHILD PROCESS THAT READ FROM 
    // A TMP FILE I THEN NEED TO READ FROM THIS TMP FILE AND WRITE THAT
    // ONTO MY WEBSOCKET
    // THEN WE ARE ALL DONE EVERYTHING ELSE IS JUST SECURITY AND WEIRD ENV STUFF
    //I LOVE CODING

}




