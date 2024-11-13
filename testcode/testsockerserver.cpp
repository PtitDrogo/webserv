#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // Create a socket
    /*
    AF_INET = IPv4 address (xxx.xxx.xxx.xxx)
    SOCK_STREAM = indicates that you are creating a stream socket, 
    which uses the TCP protocol. (Secure and garantes order of arrival is order of sending)
    Third param = Its the protocol, its finally linked with the 2nd param. Default should be fine 99%
    
    */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    std::cout << "Socket created" << std::endl;

    // Bind the socket to a address and port
    struct sockaddr_in address;
    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY; //this param means any local address 
    //Purpose: Allows the socket to listen for connections on any IP address assigned to the machine.

    address.sin_port = htons(8080); //binds to the port;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Socket bound to port 8080" << std::endl;

    /*
    Purpose of listen():
    Prepares the socket for accepting incoming client connections.
    Once listen() is called successfully, the server can then use accept() 
    to wait for and establish connections with clients.*/
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Listening for connections" << std::endl;

    // Accept a connection

    /*
    //Accept actually creates a new socket (int fd) for that one client connection
    // both other param can be used to accept a specific connection, I am not sure
    // if it will be needed
    */
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Connection accepted" << std::endl;

    // Read from the client
    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
    std::cout << "Received: " << buffer << std::endl;

    // Send a response to the client
    const char *hello = "Hello from server";
    send(client_fd, hello, strlen(hello), 0);
    std::cout << "Response sent" << std::endl;

    // Clean up
    close(client_fd);
    close(server_fd);
    return 0;
}