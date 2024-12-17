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

static bool is_executable(const char *path);
static bool file_exists(const char *path);
static std::string getActualBody(std::string& fullbody);
static std::string get_directory_path(const std::string& full_path);
static std::string get_new_executable(const std::string& full_path);
static std::string get_extension(const std::string& full_path, Client& client);
static char **createExecveArgv(const std::string& final_path, const std::string& interpreter);
CgiHandler::CgiHandler(char * const *envp, Client& client) :
_envp(envp),
_argv(NULL),
_exit_status(-1),
_path(""),
_pid(-42),
_client(client)
{}

CgiHandler::~CgiHandler() {}


static std::string get_extension(const std::string& full_path, Client& client)
{
	size_t dot_position = full_path.find(".");

	std::string tmp_path = full_path.substr(dot_position);
	std::cout << RED << tmp_path << " is tmp path" << std::endl;
	size_t end_of_extension = tmp_path.find("?");
	std::string final_extension = tmp_path.substr(0, end_of_extension);
	std::cout << RED << final_extension << " is final_extension" << std::endl;
	

	std::map<std::string, std::string>& supported_cgis = client.getServer().getCgis();

	std::map<std::string, std::string>::iterator it = supported_cgis.find(final_extension);
	printMap(supported_cgis);
	std::cout << "Address of cgis in process is " << &supported_cgis << std::endl;
	std::cout << "Address of server in process is " << &client.getServer() << std::endl;
	if (it == supported_cgis.end())
	{	
		std::cout << "PERDU" << std::endl;
		return "";
	}
	else
	{
		return it->second;
	}
}	



bool CgiHandler::HandleCgiRequest(Client& client, const HttpRequest &request)
{
	_interpreter = get_extension(request.getPath(), client);
	std::cout << _interpreter << std::endl;
	if (_interpreter == "")
	{
		generate_html_page_error(client, "403");
		return false;
	}
	_path = "." + request.getPath();
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
	_pid = cgi_pid;
    if (cgi_pid == -1)
    {
		generate_html_page_error(client, "500");
		return false;
	}
	return true;

}

void	CgiHandler::processCgiPath(const HttpRequest &request)
{
	if (request.getMethod() == "CGI-GET")
	{
		size_t start = _path.find('?');
		if (start == std::string::npos)
			return ;
		std::string tmp_path = _path.substr(start + 1);
		if (tmp_path.empty())
			return ;
		_params["QUERY_STRING"] = tmp_path;
		_params["REQUEST_METHOD"] = "GET";
		_path = _path.substr(0, start);
	}
	else if (request.getMethod() == "CGI-POST")
	{	
		std::string full_body = request.getBody();
		_body_post = getActualBody(full_body);
		_params["REQUEST_METHOD"] = "POST";
		_params["CONTENT_LENGTH"] = toString(_body_post.size());
	}
	return ;
}

static std::string get_directory_path(const std::string& full_path) 
{
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash == std::string::npos) 
        return ".";  
    return full_path.substr(0, last_slash);
}

static std::string get_new_executable(const std::string& full_path) 
{
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash == std::string::npos) 
        return "./" + full_path;
    return "." + full_path.substr(last_slash);
}

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
    
	stat(path, &st);
    if (!S_ISREG(st.st_mode)) 
        return false;
    if (access(path, X_OK) != 0) 
	{
        return false;
    }
    return true;
}

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
		if (updated_envp[i] == NULL)
		{
			for(i = 0; i < env_count + param_count; i++) 
			{
				free(updated_envp[i]);
			}
		}
		i++;
	}
	updated_envp[i] = NULL;
	return (updated_envp);
}

void CgiHandler::freeUpdatedEnv(char **tofree, char **argv_tofree)
{
	size_t env_count = 0;
	size_t new_max_count = 0;
	while(_envp[env_count] != NULL) env_count++;
	while(tofree[new_max_count] != NULL) new_max_count++;
	
	for(size_t i = env_count; i < new_max_count; i++) 
		free(tofree[i]);
	free(tofree);

	if (argv_tofree != NULL)
	{
		free(argv_tofree[0]);
		free(argv_tofree[1]);
		free(argv_tofree);
	}
	for (int i = 3; i < 1024; i++)
		close (i);
	return ;
}

pid_t    CgiHandler::executeCGI(const HttpRequest &request)
{
    if (pipe(_pipe_in) < 0)
	{
		return -1;
	}
	
	if (pipe(_pipe_out) < 0)
	{
		close (_pipe_in[0]);
		close (_pipe_in[1]);
		return -1;
	}

    int pid = fork();
    if (pid < 0)
    {
        close (_pipe_in[0]);
		close (_pipe_in[1]);
		close (_pipe_out[0]);
		close (_pipe_out[1]);
		return pid;
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
				freeUpdatedEnv(updated_env, _argv);
				close (_pipe_in[0]);
				close (_pipe_in[1]);
				close (_pipe_out[0]);
				close (_pipe_out[1]);
				std::exit(EXECVE_FAILURE);
			}
		}	
		close(_pipe_out[0]);
		close(_pipe_out[1]);
		close(_pipe_in[0]);
		std::string script_dir = get_directory_path(_path);
    
		if (chdir(script_dir.c_str()) == -1) 
		{
			freeUpdatedEnv(updated_env, _argv);
			close (_pipe_in[1]);
			perror("chdir");
			std::exit(EXECVE_FAILURE);
		}
		std::string final_path = get_new_executable(_path);
		_argv = createExecveArgv(final_path, _interpreter);
		if (_argv != NULL)
			execve(_argv[0], _argv, updated_env);
		freeUpdatedEnv(updated_env, _argv);
		close (_pipe_in[1]);
        perror("execve");
        std::exit(EXECVE_FAILURE);
    }
    else
    {
		close (_pipe_in[0]);
		close (_pipe_in[1]);
		close (_pipe_out[1]);
    }
    return (pid);
}

static char **createExecveArgv(const std::string& final_path, const std::string& interpreter) 
{
    char** argv = (char**)calloc(3, sizeof(char*));
    if (argv == NULL) 
        return NULL;
    argv[0] = strdup(interpreter.c_str());
    argv[1] = strdup(final_path.c_str());
    argv[2] = NULL;
	if (!argv[0] || !argv[1])
	{
		free(argv[0]);
		free(argv[1]);
		return NULL;
	}
    return argv;
}

void    cgiProtocol(char *const *envp, const HttpRequest &request, Client& client, Config &conf, std::vector<struct pollfd> &fds)
{
    CgiHandler cgi(envp, client);

    if (cgi.HandleCgiRequest(client, request) == true)
    {
		int pipe_fd = cgi.getPipeOut()[0];
        
		client.setCgiPID(cgi.getPID());
        addPollFD(pipe_fd, fds);
        conf.addClient(pipe_fd, client.getServer());
        conf.getClientObject(pipe_fd).setCgiCaller(&client);
		client.setCgiCallee(&conf.getClientObject(pipe_fd));
	}
    return ;

}

int     *CgiHandler::getPipeOut() {return _pipe_out;}
int     *CgiHandler::getPipeIn() {return _pipe_in;}
pid_t   CgiHandler::getPID() {return _pid;}

bool isCgiStuff(Client& client, Config &conf, std::vector<struct pollfd> &fds, size_t i)
{
	if (client.getCgiCaller() == NULL)
		return (false);
	if (client.getCgiCaller() != NULL && fds[i].revents & (POLLIN | POLLHUP))
	{
		std::cout << GREEN << "Pipe POLLIN triggered" << RESET << std::endl;
		std::string cgi_output = readFromPipeFd(fds[i].fd);
		std::string response = httpHeaderResponse("200 OK", "text/plain", cgi_output);
		if (send(client.getCgiCaller()->getSocket(), response.c_str(), response.size(), 0) < 0)
			std::cerr << RED << "Disconnecting client after failure to send response" << RESET << std::endl;
		disconnectClient(fds, client, conf);
		return true;
	}
	return false;
}


