// #include "Webserv.hpp"
// #include "CgiHandler.hpp"
// //GET /favicon.ico HTTP/1.1 is a typical request
// //GET /hello-world.py would be a cgi request;

// /*
// Step to do a CGI:
// - Recognize that the end of the request is a CGI worthy file (.py, .php)
// - Then we Send a response and execute the cgi using fork and dup
// (Question: Do we execute the cgi then send the response ??)
// */


// CgiHandler::CgiHandler(char * const *envp) :
// _envp(envp),
// _argv({NULL}),
// _exit_status(-1),
// _path(""),
// _pid(-1)
// {}

// CgiHandler::~CgiHandler() {}


// void CgiHandler::HandleCgiRequest(const std::string& request)
// {
//     std::cout << "hi whats up cgi handler here" << std::endl;
//     _path = "./config/cgi-bin/" + request;

//     //Execute the fucking cgi;
//     int PID = fork();
//     if (PID == 0)
//     {
//         execve(_path.c_str(), _argv, _envp);
//         std::cout << "I failed to execve :(" << std::endl;
//         perror("execve");
//         //How the fuck do I clean up all the objects here ? Do I care if Leak ?
//         std::exit(EXIT_FAILURE);
//     }
// }


