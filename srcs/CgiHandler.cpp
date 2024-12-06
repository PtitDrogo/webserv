#include "Webserv.hpp"

#include <fcntl.h>

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
_pid(-42),
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
	std::cout << std::endl << std::endl << "CGI PID IS :" << cgi_pid << std::endl << std::endl;
	_pid = cgi_pid;
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
        int null_fd = open("/dev/null", O_WRONLY);
		// dup2(_pipe_in[0], STDIN_FILENO);
		dup2(_pipe_out[1], STDOUT_FILENO);
		dup2(null_fd, STDERR_FILENO);

		// close(_pipe_in[0]);
		// close(_pipe_in[1]);
		close(null_fd);
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
        std::cout << "Error executing CGI"<< std::endl;
    }
    else
    {
		int pipe_fd = cgi.getPipeOut()[0];
        
		client.setCgiPID(cgi.getPID());
		std::cout << "After setting, client PID is :" << client.getCgiPID() << std::endl;
        addPollFD(pipe_fd, fds); //add to fds
        conf.addClient(pipe_fd, client.getServer()); //add to map;
        conf.getClientObject(pipe_fd).setCgiCaller(&client); //convoluted way of getting the client we just created and adding the CGI client caller;
		client.setCgiCallee(&conf.getClientObject(pipe_fd)); //Adding the pipe as callee of the initial client;
	}
    return ;

}

int     *CgiHandler::getPipeOut() {return _pipe_out;}
int     *CgiHandler::getPipeIn() {return _pipe_in;}
pid_t   CgiHandler::getPID() {return _pid;}



bool isCgiStuff(Client& client, Config &conf, std::vector<struct pollfd> &fds, size_t i)
{
	(void)i;
	printf("Caller of current client is : %p, fds[i].revents is %i\n", client.getCgiCaller(), fds[i].revents);
	if (client.getCgiCaller() == NULL)
		return (false);
	if (client.getCgiCaller() != NULL && fds[i].revents & POLLIN)
	{
		printf("Pipe disconnected1\n");
		//I want my client caller to send the content from the cgi pipe to its websocket;
		//then we disconnect client of Pipe and all is well;

		//Test close pipe;
		
		std::string cgi_output = readFromPipeFd(fds[i].fd);
		printf("IF I LOSE IT ALL\n"); 		
		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
		printf("LOSE IT ALL\n");
		if (send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0) < 0)
			std::cout << "Couldnt send data of CGI to client, error 500" << std::endl;
		// waitpid(-1, 0, 0); // Collect the child process ressources;
		printf("WHEN THE GROUND IS SHAKING\n");
		disconnectClient(fds, client, conf);
		return true;
		// wait;
	}
	if (client.getCgiCaller() != NULL && fds[i].revents & POLLHUP)
	{
		printf("Pipe disconnected2\n");
		//I want my client caller to send the content from the cgi pipe to its websocket;
		//then we disconnect client of Pipe and all is well;

		std::string cgi_output = readFromPipeFd(fds[i].fd);
		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
		if (send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0) < 0)
			std::cout << "Couldnt send data of CGI to client, error 500" << std::endl;
		// waitpid(-1, 0, 0);
		disconnectClient(fds, client, conf);
		return true;
		// wait;
	}
	printf("exiting iscgistuff\n");
	return false;
}


