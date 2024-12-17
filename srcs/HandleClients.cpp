#include "Webserv.hpp"

static void	eraseClient(std::vector<struct pollfd> &fds, Client& client, Config& conf);

void    checkIfNewClient(std::vector<struct pollfd> &fds, size_t number_of_servers, Config &conf)
{
    for (unsigned int i = 0; i < number_of_servers; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			int client_socket = SetupClientAddress(fds[i].fd);
			addPollFD(client_socket, fds);
			conf.addClient(client_socket, conf.getServer()[i]);
		}
	}
}

int safe_poll(std::vector<struct pollfd> &fds, size_t number_of_servers)
{
    if (poll(fds.data(), fds.size(), POLL_TIMEOUT_MILISECONDS) == -1)
    {
        for (unsigned int i = 0; i < number_of_servers; i++)
		{
			close(fds[i].fd);
		}
        return FAILURE;
    }
	return SUCCESS;
}

bool	handleTimeout(Client& client, std::vector<struct pollfd> &fds, Config& conf, size_t &i)
{
	if (client.didClientTimeout() == true)
	{
		if (client.getCgiCallee() == NULL && client.getCgiCaller() == NULL)
		{
			generate_html_page_error(client, "504");
			disconnectClient(fds, client, conf);
			i--;
			return true;
		}
		if (client.getCgiCallee() != NULL)
		{
			generate_html_page_error(client, "504");
			disconnectClient(fds, client, conf);
			i--;
			return true;
		}
		if (client.getCgiCaller() != NULL)
		{
			generate_html_page_error(*client.getCgiCaller(), "504");
			disconnectClient(fds, *client.getCgiCaller(), conf);
			i--;
			return true;
		}
	}
	return false;
}

void disconnectClient(std::vector<struct pollfd> &fds, Client& client, Config& conf)
{
	if (client.getCgiCallee() != NULL)
	{
		Client *cgi_client = client.getCgiCallee();
		kill(cgi_client->getCgiPID(), SIGKILL); 
		waitpid(cgi_client->getCgiPID(), 0, 0);
		eraseClient(fds, *cgi_client, conf);
		eraseClient(fds, client, conf);
	}
	else if (client.getCgiCaller() != NULL)
	{
		Client *caller_client = client.getCgiCaller();
		kill(client.getCgiPID(), SIGKILL);
		waitpid(client.getCgiPID(), 0, 0);
		eraseClient(fds, client, conf);
		eraseClient(fds, *caller_client, conf);
	}
	else
		eraseClient(fds, client, conf);
}

static void	eraseClient(std::vector<struct pollfd> &fds, Client& client, Config& conf)
{
	std::vector<struct pollfd>::iterator it = fds.begin();
	for (; it != fds.end(); it++)
	{
		if (it->fd == client.getSocket())
			break;
	}
	close(client.getSocket());
	conf.removeClient(client.getSocket());
	fds.erase(it);
}

int	handleRecvValue(int valread)
{
	if (valread > 0)
		return (SUCCESS);
	else if (valread == 0)
		return (FAILURE);
	else
		return (FAILURE);
}


void addPollFD(int client_socket, std::vector<struct pollfd> &fds)
{
	if (client_socket != -1)
	{
		struct pollfd client_pollfd;
		client_pollfd.fd = client_socket;
		client_pollfd.events = POLLIN | POLLRDHUP | POLLHUP | POLLERR;
		client_pollfd.revents = 0;
		fds.push_back(client_pollfd);
	}
	return ;
}
