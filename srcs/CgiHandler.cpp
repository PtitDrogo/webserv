#include "Webserv.hpp"



//GET /config/cgi-bin/sleep10.py HTTP/1.1
//GET /config/cgi-bin/sleep20.py HTTP/1.1
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

CgiHandler::CgiHandler(char * const *envp, Client& client) :
_envp(envp),
_argv(NULL),
_exit_status(-1),
_path(""),
_pid(-1),
_client(client)
{}

CgiHandler::~CgiHandler() {}


bool CgiHandler::HandleCgiRequest(const HttpRequest &request)
{
    // int   status;
    // (void)request;

    //A LOT OF PARSING WILL HAPPEN HERE TO SPLIT PATH INTO EXE AND PARAMETERS
    //Does path have to take into account what root is defined as ?
    _path = "." + request.getPath();
    std::cout << "hi whats up cgi handler here path is |" << _path << "|" << std::endl;

    pid_t cgi_pid = executeCGI();
    if (cgi_pid == -1)
        return false;
    return true;

}

pid_t    CgiHandler::executeCGI()
{
    // if (pipe(_pipe_in) < 0)
	// {
    //     std::cerr << "Pipe failed, error 500 !" << std::endl;
	// 	return -1;
	// }
	if (pipe(_pipe_out) < 0)
	{
        std::cerr << "Pipe failed, error 500 !" << std::endl;
		// close(_pipe_in[0]);
		// close(_pipe_in[1]);
		return -1;
	}

    int pid = fork();
    if (pid < 0)
    {
        std::cout << "Fork failed" << std::endl; //oh no !
        return pid;
        //this will be an error 500 !
    }
    else if (pid == 0)
    {
        // dup2(_pipe_in[0], STDIN_FILENO);
		dup2(_pipe_out[1], STDOUT_FILENO);
		// close(_pipe_in[0]);
		// close(_pipe_in[1]);
		close(_pipe_out[0]);
		close(_pipe_out[1]);
        std::cerr << "about to execve" << _path.c_str() << std::endl;
		execve(_path.c_str(), _argv, _envp);
        std::cerr << "failed to execve, path was : " << _path << std::endl;
        perror("execve");
        std::exit(EXIT_FAILURE);
    }
    else
    {
        // close(_pipe_in[0]);
		// close(_pipe_in[1]);
		// close(_pipe_out[0]);
		close(_pipe_out[1]);
    }
    return (pid);
}

// pid_t    CgiHandler::executeTimeOut() const
// {
//     pid_t pidTimeOut = fork();
// 	if (pidTimeOut == -1) 
//     {
// 		std::cout << "Fork failed" << std::endl; //oh no !
// 		return pidTimeOut;
// 	}
// 	else if (pidTimeOut == 0) 
//     {
// 	    usleep(TIME_OUT_CGI_MS);
// 		std::exit(EXIT_SUCCESS);
// 	}
// 	return (pidTimeOut);
// }

//ADD client there later;
void    cgiProtocol(char *const *envp, const HttpRequest &request, Client& client, Config &conf, std::vector<struct pollfd> &fds)
{
    CgiHandler cgi(envp, client);
    // std::string response;

    if (cgi.HandleCgiRequest(request) == false)
    {
        // response = httpHeaderResponse("504 Gateway Timeout", "text/plain", "The CGI script timed out.");
        std::cout << "Timeout CGI"<< std::endl;
    }
    else
    {
		int pipe_fd = cgi.getPipeOut()[0];
        
        addPollFD(pipe_fd, fds); //add to fds
        conf.addClient(pipe_fd, client.getServer()); //add to map;
        conf.getClientObject(pipe_fd).setCgiCaller(&client); //convoluted way of getting the client we just created and adding the CGI client caller;
    }
    return ;

}

int     *CgiHandler::getPipeOut() {return _pipe_out;}
int     *CgiHandler::getPipeIn() {return _pipe_in;}



