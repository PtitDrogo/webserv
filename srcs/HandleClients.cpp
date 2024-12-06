#include "Webserv.hpp"


//Check if a new client wants to connect to our server
void    checkIfNewClient(std::vector<struct pollfd> &fds, size_t number_of_servers, Config &conf)
{
    for (unsigned int i = 0; i < number_of_servers; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			int client_socket = SetupClientAddress(fds[i].fd);
			addPollFD(client_socket, fds);
			printf("DEBUG: GONNA Add client to the list\n");
			conf.addClient(client_socket, conf.getServer()[i]);
			//Ici je le rajoute a la liste des fd;
			printf("DEBUG: Added client to the list\n");
			//Technically cleaner with "continue ;" here, but it doesnt work somehow
			// if we do that and its not necessary
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
		std::cerr << "Poll failed" << std::endl;
        for (unsigned int i = 0; i < number_of_servers; i++)
		{
			close(fds[i].fd); //do we have to do more cleanup ? I dont even know
		}
        return FAILURE; //Question : Est ce que on doit vraiment quitter si poll fail ? Surement oui mais a voir.
    }
	return SUCCESS;
}

//Parametres -> la liste de fds et l'index du client a deconnect
// void disconnectClient(std::vector<struct pollfd> &fds, size_t &i, Config& conf)
// {
// 	std::cout << "Client disconnected" << std::endl;
// 	close(fds[i].fd);
// 	conf.removeClient(fds[i].fd); //Remove the client from the map of conf
// 	fds.erase(fds.begin() + i);   //Remove the client from the vector of pollfds;
// 	--i;
// }

void disconnectClient(std::vector<struct pollfd> &fds, Client& client, Config& conf)
{
	std::cout << "Client disconnected" << std::endl;
	std::vector<struct pollfd>::iterator it = fds.begin();
	if (client.getCgiCallee() != NULL)
	{	
		std::cout << std::endl << "I love killing pid :" << client.getCgiPID() << std::endl;
		disconnectClient(fds, *client.getCgiCallee(), conf); //disconnecting the pipe of cgi if it exists;
		kill(client.getCgiCallee()->getCgiPID(), SIGKILL); //calling kill on zombie does nothing, woohoo !
		waitpid(client.getCgiCallee()->getCgiPID(), 0, 0);	
	}
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
int	handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds, Config& conf)
{
	(void)i;
	(void)fds;
	(void)conf;
	if (valread > 0)
	{
		// std::cout << "DEBUG:Received from client successfully" << std::endl;
		return (SUCCESS);
	}
	else if (valread == 0)
	{
		std::cout << "DEBUG:Recve detected no client, disconnecting" << std::endl;
		// disconnectClient(fds, i, conf);
		return (FAILURE); //In theory this should never trigger but leaving just in case
	}
	else
	{
		std::cerr << "Error reading from client" << std::endl;
		// disconnectClient(fds, i, conf);
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

