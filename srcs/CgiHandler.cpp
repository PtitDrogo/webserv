#include "Webserv.hpp"

#include <fcntl.h>
#include <sys/stat.h>

//GET /config/cgi-bin/sleep10.py HTTP/1.1
//GET /config/cgi-bin/sleep20.py HTTP/1.1
//GET /config/cgi-bin/time.py HTTP/1.1
//GET /config/cgi-bin/helloworld.py HTTP/1.1
//GET /config/cgi-bin/helloworld.py?name=theo&age=29 HTTP/1.1
//GET /favicon.ico HTTP/1.1 is a typical request
//GET /hello-world.py would be a cgi request;
//GET /config/cgi-bin/basicparam.py?name=theo&age=29 HTTP/1.1

//GET /config/cgi-bin/helloworld.py HTTP/1.1

//GET /config/cgi-bin/printquerystring.py?name=theo&age=29 HTTP/1.1

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


static bool is_executable(const char *path);
static bool file_exists(const char *path);
static std::string getActualBody(std::string& fullbody);
static std::string get_directory_path(const std::string& full_path); 

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
	_path = "." + request.getPath(); //Sometime Ill need a dot, sometime not, exciting !
	// std::cout << "Path is : " << _path << std::endl;
	processCgiPath(request);
	if (file_exists(_path.c_str()) == false)
	{
		generate_html_page_error(_client, "404");
		return false;
	}	
	if (is_executable(_path.c_str()) == false)
	{
		generate_html_page_error(_client, "403");
		return false;
	}
    pid_t cgi_pid = executeCGI(request);
	// std::cout << std::endl << std::endl << "CGI PID IS :" << cgi_pid << std::endl << std::endl;
	_pid = cgi_pid;
    if (cgi_pid == -1)
        return false;
    return true;

}

//This gets the real path of the cgi and extract all parameters into a map;
void	CgiHandler::processCgiPath(const HttpRequest &request)
{
	if (request.getMethod() == "CGI-GET")
	{
		size_t start = _path.find('?');
		if (start == std::string::npos)
			return ; //This means theres no parameters, we fuck off;
		std::string tmp_path = _path.substr(start + 1);
		if (tmp_path.empty())
			return ; //to be safe
		_params["QUERY_STRING"] = tmp_path;
		_params["REQUEST_METHOD"] = "GET";
		_path = _path.substr(0, start); //Setting the actual path has the entire string thats before the ?
		_params["PATH_INFO"] = _path;
	}
	else if (request.getMethod() == "CGI-POST")
	{	
		std::string full_body = request.getBody();
		_body_post = getActualBody(full_body);
		_params["REQUEST_METHOD"] = "POST";
		_params["CONTENT_LENGTH"] = toString(_body_post.size());
		_params["PATH_INFO"] = _path;
		// std::cout << GREEN << "ACTUAL VARIABLE = " << getActualBody(full_body) << std::endl;
		// std::cout << GREEN << "BODY LENGTH = " << _client.getContentLength() << std::endl;
	}
	return ;
}

// for ./config/cgi-bin/hello.py, this will get ./config/cgi-bin/, so we can them move into that directory
static std::string get_directory_path(const std::string& full_path) 
{
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash == std::string::npos) 
        return ".";  
    return full_path.substr(0, last_slash);
}

//As it stands in the code, "body" is just the entire request, so this gets what i actually care about
static std::string getActualBody(std::string& fullbody) 
{
    std::string line;
    std::istringstream stream(fullbody);
    std::string body;
    bool found_empty_line = false;

    while (std::getline(stream, line)) 
	{
        if (!line.empty() && line[line.size() - 1] == '\r') 
            line = line.substr(0, line.size() - 1);

        if (line.empty() && !found_empty_line)
		{
            found_empty_line = true;
            continue;
		}
        if (found_empty_line)
            body += line + "\n";
    }
    if (!body.empty() && body[body.size() - 1] == '\n') 
        body = body.substr(0, body.size() - 1);
    return body;
}


static bool file_exists(const char *path)
{
	struct stat st;
    
    if (stat(path, &st) != 0)
		return false;
	return true;
}

static bool is_executable(const char *path) 
{
	struct stat st;
    
    // Is it a file
	stat(path, &st);
    if (!S_ISREG(st.st_mode)) 
        return false;
    //is it exe
    if (access(path, X_OK) != 0) 
	{
        return false;
    }
    return true;
}

//Returns an updated env equal to envp + parameters of the cgi request
char **CgiHandler::updateEnv()
{
	size_t env_count = 0;
	size_t param_count = _params.size();
	size_t i;

    while(_envp[env_count] != NULL) env_count++;
    
    char** updated_envp = (char**)calloc((env_count + param_count) + 1, sizeof(char*));
	if (updated_envp == NULL)
		return NULL;
    for(i = 0; i < env_count; i++) 
	{
        updated_envp[i] = _envp[i];
    }
	
	for (std::map<std::string, std::string>::iterator it = _params.begin(); it != _params.end(); it++)
	{
		std::string new_var;

		new_var = it->first + "=" + it->second;
		updated_envp[i] = strdup(new_var.c_str());
		// std::cout << "Added : " << updated_envp[i] << ", to the envp " << std::endl;
		if (updated_envp[i] == NULL)
		{
			for(i = 0; i < env_count + param_count; i++) 
			{
				free(updated_envp[i]); // With the power of calloc this ok actualy memory safe;
			}
		}
		i++;
	}
	updated_envp[i] = NULL;
	return (updated_envp);
}

void CgiHandler::freeUpdatedEnv(char **tofree)
{
	size_t env_count = 0;
	size_t new_max_count = 0;
	while(_envp[env_count] != NULL) env_count++;
	while(tofree[new_max_count] != NULL) new_max_count++;
	
	for(size_t i = env_count; i < new_max_count; i++) 
		free(tofree[i]);
	free(tofree);
	return ;
}

pid_t    CgiHandler::executeCGI(const HttpRequest &request)
{
    if (pipe(_pipe_in) < 0)
	{
        std::cerr << "Pipe failed, error 500 !" << std::endl;
		return -1;
	}
	
	if (pipe(_pipe_out) < 0)
	{
        std::cerr << "Pipe failed, sending back error 500 !" << std::endl;
		close (_pipe_in[0]);
		close (_pipe_in[1]);
		return -1;
	}

    int pid = fork();
    if (pid < 0)
    {
        std::cout << "Fork failed" << std::endl; //oh no !
        close (_pipe_in[0]);
		close (_pipe_in[1]);
		close (_pipe_out[0]);
		close (_pipe_out[1]);
		return pid;
        //this will be an error 500 !
    }
    else if (pid == 0)
    {
        char **updated_env = updateEnv();
		dup2(_pipe_out[1], STDOUT_FILENO);
		dup2(_pipe_in[0], STDIN_FILENO);
		int null_fd = open("/dev/null", O_WRONLY);
		dup2(null_fd, STDERR_FILENO);
		close(null_fd);
		if (request.getMethod() == "CGI-POST")
		{
			if (write(_pipe_in[1], _body_post.c_str(), _body_post.size()) == -1)
			{
				freeUpdatedEnv(updated_env);
				close (_pipe_in[0]);
				close (_pipe_in[1]);
				close (_pipe_out[0]);
				close (_pipe_out[1]);
				std::exit(EXIT_FAILURE);
			}
		}	
		close(_pipe_out[0]);
		close(_pipe_out[1]);
		close(_pipe_in[0]);
        std::cerr << "about to execve" << _path.c_str() << std::endl;
		std::string script_dir = get_directory_path(_path);
    
		if (chdir(script_dir.c_str()) == -1) 
		{
			freeUpdatedEnv(updated_env);
			close (_pipe_in[1]);
			perror("chdir");
			std::exit(EXIT_FAILURE);
		}
		execve(_path.c_str(), _argv, updated_env);
        std::cerr << RED << "failed to execve, path was : " << _path << RESET << std::endl;
		freeUpdatedEnv(updated_env);
		close (_pipe_in[1]);
        perror("execve");
        std::exit(EXECVE_FAILURE); //st
    }
    else
    {
		close (_pipe_in[0]);
		close (_pipe_in[1]);
		close (_pipe_out[1]);
    }
    return (pid);
}

//ADD client there later;
void    cgiProtocol(char *const *envp, const HttpRequest &request, Client& client, Config &conf, std::vector<struct pollfd> &fds)
{
    CgiHandler cgi(envp, client);

    if (cgi.HandleCgiRequest(request) == false)
    {
        // response = httpHeaderResponse("504 Gateway Timeout", "text/plain", "The CGI script timed out.");
        std::cout << "Error executing CGI"<< std::endl;
    }
    else
    {
		int pipe_fd = cgi.getPipeOut()[0];
        
		client.setCgiPID(cgi.getPID());
		// std::cout << "After setting, client PID is :" << client.getCgiPID() << std::endl;
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
	if (client.getCgiCaller() == NULL)
		return (false);
	if (client.getCgiCaller() != NULL && fds[i].revents & POLLIN)
	{
		std::cout << GREEN << "Pipe POLLIN triggered" << RESET << std::endl;
		//checking to see if the process didnt exit failure (aka execve failed);
		int status;
		waitpid(client.getCgiCaller()->getCgiPID(), &status, WNOHANG);
		std::cout << "hi boys " << std::endl;
		if (WIFEXITED(status)) 
		{
			std::cout << "hi again" << std::endl;
			int exit_code = WEXITSTATUS(status);
			if (exit_code == EXECVE_FAILURE)
			{
				std::cout << "Victory !" << std::endl;
				generate_html_page_error(client, "500");
				disconnectClient(fds, client, conf);
				return true;
			}
		}
		std::string cgi_output = readFromPipeFd(fds[i].fd);
		//Check process status here, and if its bad, send a error 500.	
		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
		if (send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0) < 0)
			std::cerr << "Couldnt send data of CGI to client, error 500" << std::endl;
		disconnectClient(fds, client, conf);
		return true;
	}
	if (client.getCgiCaller() != NULL && fds[i].revents & POLLHUP) //I have never seen this trigger.
	{
		printf("Pipe disconnected2\n");
		//I want my client caller to send the content from the cgi pipe to its websocket;
		//then we disconnect client of Pipe and all is well;

		std::string cgi_output = readFromPipeFd(fds[i].fd);
		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
		if (send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0) < 0)
			std::cout << "Couldnt send data of CGI to client, error 500" << std::endl;
		disconnectClient(fds, client, conf);
		return true;
	}
	return false;
}


