#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    std::cout << "Socket created" << std::endl;

    // Bind the socket to a address and port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Socket bound to port 8080" << std::endl;

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Listening for connections" << std::endl;

    // Accept a connection
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