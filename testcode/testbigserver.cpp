#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>

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

    // Bind the socket to an address and port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections on any local IP
    address.sin_port = htons(8080);       // Bind to port 8080

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

    // Prepare the pollfd structure
    std::vector<struct pollfd> fds;
    
    //creating the server_poll
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    fds.push_back(server_pollfd); // Add the server socket for listening

    while (true)
    {
        // Poll for events on the sockets
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0)
        {
            std::cerr << "Poll failed" << std::endl;
            close(server_fd);
            return 1;
        }

        // Check if the server socket is ready to accept a new connection
        if (fds[0].revents & POLLIN)
        {
            struct sockaddr_in client_address;
            socklen_t addr_len = sizeof(client_address);
            int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &addr_len);
            if (client_fd < 0)
            {
                std::cerr << "Accept failed" << std::endl;
                continue; // Try again
            }
            std::cout << "New connection accepted" << std::endl;

            // Add the new client socket to the pollfd array
            fds.push_back({client_fd, POLLIN, 0});
        }

        // Check all client sockets for incoming data
        for (size_t i = 1; i < fds.size(); ++i)
        { // Start from index 1 (skip the server_fd)
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024] = {0};
                int valread = read(fds[i].fd, buffer, 1024);
                if (valread > 0)
                {
                    std::cout << "Received from client: " << buffer << std::endl;

                    // Send a response to the client
                    const char *response = "Hello from server";
                    send(fds[i].fd, response, strlen(response), 0);
                    std::cout << "Response sent" << std::endl;
                }
                else if (valread == 0)
                {
                    // Client disconnected
                    std::cout << "Client disconnected" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i; // Adjust the index to account for the removed client
                }
                else
                {
                    // Error reading from the client
                    std::cerr << "Error reading from client" << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i;
                }
            }
        }
    }

    // Clean up (will never be reached due to infinite loop)
    close(server_fd);
    return 0;
}
