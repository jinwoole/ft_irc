#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 6667;
const int MAX_CLIENTS = 10;


int main()
{
    int client_sockfd;
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Bind the socket to a port
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    while (1)
    {
        listen(sockfd, MAX_CLIENTS);

        // Accept a connection from a client
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

        if (client_sockfd < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            return 1;
        }

        // Receive messages from the client
        char buffer[256];
        std::memset(buffer, 0, sizeof(buffer));

        int n = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0)
        {
            std::cerr << "Error receiving message" << std::endl;
            return 1;
        }

        std::cout << "Received message: " << buffer << std::endl;
    }

    // Close the sockets
    close(client_sockfd);
    close(sockfd);

    return 0;
}