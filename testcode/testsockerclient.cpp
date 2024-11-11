#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    std::cout << "Socket created" << std::endl;

    // Connect to the server
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.2");
    address.sin_port = htons(1080);

    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Connect failed" << std::endl;
        close(client_fd);
        return 1;
    }
    std::cout << "Connected to the server" << std::endl;

    // Keep the client running for continuous communication
    while (true)
    {
        std::string user_input;
        std::cout << "Enter a message to send to the server (type 'exit' to quit): ";
        std::getline(std::cin, user_input);

        // Exit condition
        if (user_input == "exit")
        {
            break;
        }

        // Send the user input to the server
        send(client_fd, user_input.c_str(), user_input.length(), 0);
        std::cout << "Message sent" << std::endl;

        // Read the response from the server
        char buffer[1024] = {0};
        int valread = read(client_fd, buffer, 1024);
        if (valread > 0)
        {
            std::cout << "Received: " << buffer << std::endl;
        }
        else if (valread == 0)
        {
            std::cout << "Server closed the connection" << std::endl;
            break;
        }
        else
        {
            std::cerr << "Error reading from server" << std::endl;
            break;
        }
    }

    // Clean up
    close(client_fd);
    std::cout << "Client closed" << std::endl;
    return 0;
}
