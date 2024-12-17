#include "Webserv.hpp"
#include "Cookies.hpp"

int main(int argc, char **argv, char **envp)
{
	Config conf;

	if (argc != 2)
		return FAILURE;
	if (conf.parse_config_file(argv[1]) == false)
		return FAILURE;
	try
	{
		int status = execute_server(conf, envp);
		return status;
	}
	catch(const std::exception& e)
	{
		return FAILURE;
	}
	return SUCCESS;
}






