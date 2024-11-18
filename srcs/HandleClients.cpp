#include "include.hpp"
#include "Webserv.hpp"


//Check if a new client wants to connect to our server
void    checkIfNewClient(std::vector<struct pollfd> &fds, int server_socket)
{
    if (fds[0].revents & POLLIN)
    {
        int client_socket = SetupClientAddress(server_socket);
        addPollFD(client_socket, fds);
        printf("DEBUG: Added client to the list\n");
        //Technically cleaner with "continue ;" here, but it doesnt work somehow
		// if we do that and its not necessary
    }
}
//Checks all clients sockets to see if something happened
int safe_poll(std::vector<struct pollfd> &fds, int server_socket)
{
    if (poll(fds.data(), fds.size(), -1) == -1)
    {
        std::cerr << "Poll failed" << std::endl;
        close(server_socket); //technically would have to do more cleanup than that;
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

