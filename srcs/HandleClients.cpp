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
			conf.addClient(client_socket, i);
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
    if (poll(fds.data(), fds.size(), POLL_TIMEOUT) == -1) //
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
void disconnectClient(std::vector<struct pollfd> &fds, size_t &i)
{
	std::cout << "Client disconnected" << std::endl;
	close(fds[i].fd);
	fds.erase(fds.begin() + i);
	--i;
}


//Parametres -> retour de recv, la liste de fds et l'index du client (pour deconnect sur fail)
int	handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds)
{

	if (valread > 0)
	{
		// std::cout << "DEBUG:Received from client successfully" << std::endl;
		return (SUCCESS);
	}
	else if (valread == 0)
	{
		
		std::cout << "DEBUG:Recve detected no client, disconnecting" << std::endl;
		disconnectClient(fds, i);
		return (FAILURE); //In theory this should never trigger but leaving just in case
	}
	else
	{
		std::cerr << "Error reading from client" << std::endl;
		disconnectClient(fds, i);
		return (FAILURE);
	}
}

void addPollFD(int client_socket, std::vector<struct pollfd> &fds)
{
	if (client_socket != -1)
	{
		struct pollfd client_pollfd;
		client_pollfd.fd = client_socket;
		client_pollfd.events = POLLIN | POLLRDHUP; //POLLIN = Un truc happened sur la socket POLLRDHUP = plus de client
		client_pollfd.revents = 0;
		fds.push_back(client_pollfd);
	}
	return ;
}

