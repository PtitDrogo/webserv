#include "Webserv.hpp"


int SetupClientAddress(int server_socket)
{
	///////////////////////////////
	/////////ACCEPT////////////////
	///////////////////////////////
	sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);
	int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
	if (client_socket == -1)
	{
		std::cerr << "Erreur lors de l'acceptation de la connexion." << std::endl;
		close(server_socket);
		return -1;
	}
	return client_socket;
}

