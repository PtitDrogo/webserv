#include "Webserv.hpp"

static void	eraseClient(std::vector<struct pollfd> &fds, Client& client, Config& conf);

//Check if a new client wants to connect to our server
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
//Checks all clients sockets to see if something happened
//In this function we are also polling the server, should we do that ??
int safe_poll(std::vector<struct pollfd> &fds, size_t number_of_servers)
{
    if (poll(fds.data(), fds.size(), POLL_TIMEOUT_MILISECONDS) == -1) //POLL_TIMEOUT
    {
        //NOTE : if we use ctrlC or ctrl Z, this will print this, before pushing to prod i could check the static variable to know that its normal to fail this and not print.
		if (Config::ServerRunning == true)
			std::cerr << "Poll failed" << std::endl;
        for (unsigned int i = 0; i < number_of_servers; i++)
		{
			close(fds[i].fd); //do we have to do more cleanup ? I dont even know
		}
        return FAILURE; //Question : Est ce que on doit vraiment quitter si poll fail ? Surement oui mais a voir.
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

//Handle a bunch of logic regarding Pipes of Cgis before calling the disconnect of one (or two !) clients.
void disconnectClient(std::vector<struct pollfd> &fds, Client& client, Config& conf)
{
	std::cout << "Client disconnected" << std::endl;
	if (client.getCgiCallee() != NULL)
	{
		Client *cgi_client = client.getCgiCallee();
		kill(cgi_client->getCgiPID(), SIGKILL); //calling kill on zombie does nothing, woohoo !
		waitpid(cgi_client->getCgiPID(), 0, 0);
		eraseClient(fds, *cgi_client, conf); //killing cgi client
		eraseClient(fds, client, conf); //killing current client
	}
	else if (client.getCgiCaller() != NULL)
	{
		Client *caller_client = client.getCgiCaller();
		kill(client.getCgiPID(), SIGKILL); //calling kill on zombie does nothing, woohoo !
		waitpid(client.getCgiPID(), 0, 0);
		eraseClient(fds, client, conf); //killing current client
		eraseClient(fds, *caller_client, conf); //killing cgi client
	}
	else
	{
		eraseClient(fds, client, conf);
	}
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
	conf.removeClient(client.getSocket()); //Remove the client from the map of conf
	fds.erase(it);
}


//Parametres -> retour de recv, la liste de fds et l'index du client (pour deconnect sur fail)
int	handleRecvValue(int valread)
{
	if (valread > 0)
	{
		return (SUCCESS);
	}
	else if (valread == 0)
	{
		std::cout << "DEBUG:Recve detected no client, disconnecting" << std::endl;
		return (FAILURE);
	}
	else
	{
		std::cerr << "Error reading from client" << std::endl;
		return (FAILURE);
	}
}


//add POLLHUP later;
void addPollFD(int client_socket, std::vector<struct pollfd> &fds)
{
	if (client_socket != -1)
	{
		struct pollfd client_pollfd;
		client_pollfd.fd = client_socket;
		//POLLHUP because im going mad debugging this, but later will be useful for pipe
		client_pollfd.events = POLLIN | POLLRDHUP | POLLHUP | POLLERR; //POLLIN = Un truc happened sur la socket POLLRDHUP = plus de client // NOT ADDING POLLOUT because it makes loop go infinite for now
		client_pollfd.revents = 0;
		fds.push_back(client_pollfd);
	}
	return ;
}


------WebKitFormBoundaryB7368rYhJmiWnTwY--
